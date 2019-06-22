



#include "pch.h"


#include "Hvy3DScene.h"
#include "..\Common\DirectXHelper.h"

#include <ppltasks.h>




using namespace concurrency;

using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;


using namespace HvyDXBase; 
using namespace DirectX;
using namespace Windows::Foundation;













void Hvy3DScene::GoFileOpenPicker()
{
    auto openPicker = ref new FileOpenPicker();

    openPicker->ViewMode = PickerViewMode::Thumbnail; 

    openPicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary; 


    openPicker->FileTypeFilter->Append(".jpg"); 
    openPicker->FileTypeFilter->Append(".png"); 



    create_task(openPicker->PickSingleFileAsync()).then([this](StorageFile^ file) {

        if (file)
        {
            auto tempFolder = ApplicationData::Current->TemporaryFolder;
            create_task(file->CopyAsync( tempFolder, file->Name, NameCollisionOption::GenerateUniqueName )).then([this](StorageFile^ tempFile)
            {
                if (tempFile)
                {
                    Microsoft::WRL::ComPtr<ID3D11Resource>  temp_resource;


                    HRESULT hr = CreateWICTextureFromFile(
                        m_deviceResources->GetD3DDevice(), 
                        tempFile->Path->Data(), 
                        temp_resource.ReleaseAndGetAddressOf(),
                        e_srv_FunDomain.ReleaseAndGetAddressOf(), 
                        0
                    );

                    DeleteFile(tempFile->Path->Data());

                    DX::ThrowIfFailed(hr);
                }
            }); 
        }
    });

}











# RGno
Hyperbolic tessellations in the Poincare Disk model using DirectX D3D11 on Windows 10. 
This is a complete C++ project for Microsoft Visual Studio 17 to build a UWP DirectX application targeting Windows 10. 

Much of the mathematics behind the hyperbolic geometry in this project is based upon work by Arnaud Cheritat. See "Universal Cover of the Klein Quartic" at  https://www.math.univ-toulouse.fr/~cheritat/AppletsDivers/Klein/. 

Use keyboard keys F2, F3, F4 and F5 to change the Schlafli Symbol of the tessellation to the corresponding pre-defined value. 
Use the keyboard "O" key to open a file picker. The file chosen will be used as the new Fundamental Domain for the tessellation. 
Use the keyboard "I" key to toggle color inversion of the hyperbolic tessellation. 


Dependencies: The RGno project depends on Microsoft DirectXTK. RGno uses the nuget functionality in VisualStudio (Tools -> NuGet Package Manager -> Package Manager Console) to manage dependencies. See the "packages.config" file in this repository. 

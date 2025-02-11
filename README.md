This repo consists of C++ code to implement a ray tracer. Part of CGR course at UoE. 
All the required JSON files are present inside Code->Json

1. To compile the c++ file, run the Makefile by typing    make  or    make raytracer

2. Once the files are compiled successfully, it can be executed as follows     ./raytracer file_name.json

3. All the 4 given JSON files are in the same directory as the code.

4. Execution code to copy<br/>
        -    ```./raytracer binary_primitives.json<br/>```
        -    ./raytracer simple_phong.json<br/>
        -    ./raytracer scene.json<br/>
        -    ./raytracer mirror_image.json<br/>

5. Once the json file in inputted, the program gives an opton to render the image in either using Blinn-Phong shading method or Normal Binary shading method.

6. To render using Blinn-Phong shading, press 2

7. To render using normal binary shading, press 1

8. It takes around 5 to 15 seconds to render based on the json file and the chosen mode

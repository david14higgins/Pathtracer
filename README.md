# Execution Instructions

1. `cd` to the `Code` directory containing the Makefile
2. run `make` to compile the program
3. run `.\raytracer.exe "<scene_name>.json"` with any additional flags

Scene jsons can be found in the `sceneJsons` directory. Scene will be written to the `renders` directory with file name `"<scene_name>.ppm"`

If any issues, run `make clean` then repeat steps 2 and 3. 

# Flags

- `--useBVH` use a BVH for intersection tests
- `--useAA` use antialiasing

All other features are read from the scene json file

# Example 

1. `cd Code`
2. `make`
3. `.\raytracer.exe "phong_scene.json" --useAA` 





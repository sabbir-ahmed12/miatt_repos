# TODOs:

- [ ] 15pts QUESTIONS/TASK.md
- [ ] 35pts ComputeInterPupillaryDistance/TASK.md
- [ ] 50pts EdgeSlicerModule/TASK.md  (You will need to modify the CMakeLists.txt files, and source files.)


HINTS:
The `XXXX` and `TODO` designations in the reference code indicate one
of many possible solutions.  These designations indicate that code or 
documentation is needed to make a complete solution.

        -- XXXX is a hint.  It is not required that you put something there.
        -- XXXX is a hint.  It does not indicate that those are the only places that need content added.
        -- There are relatively few code line changes to be made.  You should not be writing 100's of lines of code.
        -- `ITK_LIBRARIES` is an important CMake variable that you may need
        -- `ITK_DIR=/nfsscratch/opt/ece5490/Linux/Ubuntu/bionic/ITK-bld`
        -- `SlicerExecutionModel_DIR=/nfsscratch/opt/ece5490/Linux/Ubuntu/bionic/SEM-bld`

```bash
cmake -G Ninja \
  -DITK_DIR=/nfsscratch/opt/ece5490/Linux/Ubuntu/bionic/ITK-bld \
  -DSlicerExecutionModel_DIR=/nfsscratch/opt/ece5490/Linux/Ubuntu/bionic/SEM-bld \
  ..
```

Think before you type.  Guessing will likely take you down very bad paths to wrong answers.

Commit early, Commit often.

It is more important to convey what you know than to get exactly the correct answer.
        
        -- Not giving an answer has a 100% chance of earning a maximum of 0 points
        -- Adding a comment that describes what you would do allows earning of 
           partial credit.  BE SPECIFIC and use the proper terminology.

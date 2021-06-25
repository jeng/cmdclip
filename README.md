# CMD CLIP
## Overview
This is a port of command line utilities that I use all the time.  The originals were written in Delphi back in 2003.  I wanted a version that I could compile on modern machines so I ported it to C++ using vanilla Windows' system calls. 

- The command `dumpclip` writes the clipboard to stdout.
- The command `pushclip` takes piped data and sends it to the clipboard.

It is useful to chain them together adding some type of filter between the two.  Here are some examples:

Save the list of 5 letter words from a large dataset that begin with `wor` to the clipboard:

    C:\Users\Jeremy\src\cmdclip             
    λ grep -ri "^wor..$" word.lst | pushclip
                                            
    C:\Users\Jeremy\src\cmdclip             
    λ dumpclip                              
    words                                   
    wordy                                   
    works                                   
    world                                   
    worms                                   
    wormy                                   
    worry                                   
    worse                                   
    worst                                   
    worth                                   
    worts                                   


Make those words snakecase:

    C:\Users\Jeremy\src\cmdclip                                               
    λ dumpclip | sed -rn 's/(.)(.)(.)(.)(.)/\U\1\L\2\U\3\L\4\U\3/p' | pushclip
                                                                            
    C:\Users\Jeremy\src\cmdclip                                               
    λ dumpclip                                                                
    WoRdR                                                                     
    WoRdR                                                                     
    WoRkR                                                                     
    WoRlR                                                                     
    WoRmR                                                                     
    WoRmR                                                                     
    WoRrR                                                                     
    WoRsR                                                                     
    WoRsR                                                                     
    WoRtR                                                                     
    WoRtR                                                                     


## Build and installation instructions
- Start a shell with the Visual Studio environment variables defined
- Run build.bat
- Copy the executables to a location on your PATH

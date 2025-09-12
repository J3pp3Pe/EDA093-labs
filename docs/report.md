In what order we did the lab:
execute a program ("failed attempt")
getcwd
execute a program

The first thing we did on this lab was to get things to execute properly. We made a execute_cmd() function, which utilized fork(), and we tried to make the child execute using execvp().
After some failed attempts at gettings "ls" to work we realized that we have no idea of what's going on and where we are. So we couldn't know if "ls" would print anything.
So we implemented the getcwd function in our code so we could get some idea what was going on. This part could have waited, but normally when working in bash or any other shell it's nice to have an idea of where you are working on the system, so we implemented it early on.

After getting getcwd to work we started to work on the part that matters, to get programs to execute. This time around we realized that the only thing missing was the indexing of "pgmlist". When we realized that part we got a better understanding of how we will handle multiple calls in one line.
In what order we did the lab:
execute a program ("failed attempt")
getcwd
execute a program

The first thing we did on this lab was to get things to execute properly. We made a execute_cmd() function, which utilized fork(), and we tried to make the child execute using execvp().
After some failed attempts at gettings "ls" to work we realized that we have no idea of what's going on and where we are. So we couldn't know if "ls" would print anything.
So we implemented the getcwd function in our code so we could get some idea what was going on. This part could have waited, but normally when working in bash or any other shell it's nice to have an idea of where you are working on the system, so we implemented it early on.

After getting getcwd to work we started to work on the part that matters, to get programs to execute. This time around we realized that the only thing missing was the indexing of "pgmlist". When we realized that part we got a better understanding of how we will handle multiple calls in one line.

Then we started on getting CTRL-D (EOF) to work. We used a quick solution which only checked line == null. Which seems to work fairly well.

Our next objective was getting running processes in the background to work. Since we already had the code for forking and waiting, all we had to do was add a if check that checks if the background bit in the Command struct was 1. If it is set to 1, we simply don't wait, and continue executing.
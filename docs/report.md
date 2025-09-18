In what order we did the lab:
execute a program ("failed attempt")
getcwd
execute a program
ctrl-d EOF
background processess
pipe (not really)
ctrl-c sigint (partial)
pipe
redirects

The first thing we did on this lab was to get things to execute properly. We made a execute_cmd() function, which utilized fork(), and we tried to make the child execute using execvp().
After some failed attempts at gettings "ls" to work we realized that we have no idea of what's going on and where we are. So we couldn't know if "ls" would print anything.
So we implemented the getcwd function in our code so we could get some idea what was going on. This part could have waited, but normally when working in bash or any other shell it's nice to have an idea of where you are working on the system, so we implemented it early on.

After getting getcwd to work we started to work on the part that matters, to get programs to execute. This time around we realized that the only thing missing was the indexing of "pgmlist". When we realized that part we got a better understanding of how we will handle multiple calls in one line.

Then we started on getting CTRL-D (EOF) to work. We used a quick solution which only checked line == null. Which seems to work fairly well.

Our next objective was getting running processes in the background to work. Since we already had the code for forking and waiting, all we had to do was add a if check that checks if the background bit in the Command struct was 1. If it is set to 1, we simply don't wait, and continue executing.

The next functionality on the list was pipes, so we started to work on that and got it to work fairly well. Since we don't have anything to handle arguments, it gets messy with multiple function calls. Seeing what needs to be fixed was kind of difficult this way.
So we decided to move on for now, as we need to implement function arguments first.

Since we didn't have to much time left on the lab session we decided to implement something quick. So we decided to implement sigint (ctrl-c). We quickly got it to work, but the ctrl-c behaviour was a bit to aggressive, it killed the entire shell. So we had to fix the sig_handler function to work for our usecase.

At this point i realized that I had misunderstood the piping, and everything I did with piping had to be done over.

At an even later point I realized that I didn't completly misunderstand piping, and that some logic would still work.
After some trial and error with all fds et.c it seems to work. There's still some work needed to be done with error handling, and probably some edge cases. But the basic functionality of piping works.

We noticed that redirects only worked when we had pipes, which was due to a problem with out loop logic. This was quickly fixed, but we noticed that redirects was still giving us some problems, this was caused due to us not having the appropriate arguments/flags to the open() function.
Another problem we had to overcome, was the fact that we iterated through the command list in the "wrong order". We fixed this by reversing the linked list by using a temporary stack.
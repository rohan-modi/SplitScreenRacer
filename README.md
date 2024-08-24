# ECE243Project
This is a two player split screen racing game written in C, meant to be loaded as a single file and played on a DE1-SoC board. 

Required hardware: Speakers, a VGA monitor, and a PS2 keyboard

View a video demonstration of the game at the link below, loaded onto a DE1-SoC board.

https://drive.google.com/file/d/1k5xWr4A1fzrSyDHc-mw7_wvZRGdILH_B/view?usp=sharing


Visit the link below to simulate the DE1-SoC board, and all attached hardware

https://cpulator.01xz.net/?sys=nios-de1soc

<h1>Simulation Instructions</h1>

<p><strong>Initial Simulator Screen:</strong></p>
<img width="1440" alt="Initial Simulator Screen" src="https://github.com/user-attachments/assets/2073d50a-b04d-4fe0-909c-46e9699f382d">
<p>This is what the simulator looks like initially upon loading the page.</p>

<p><strong>Change Language to C:</strong></p>
<img width="478" alt="Change Language to C" src="https://github.com/user-attachments/assets/5195e765-9757-41e0-a73a-4f5bfe0c2849">
<p>In the top center of the screen, change the language to C.</p>

<p><strong>Delete Starting Code and Paste Code:</strong></p>
<img width="937" alt="Delete Starting Code and Paste Code" src="https://github.com/user-attachments/assets/b82bfd62-17ef-4759-b107-a126b93c2115">
<p>Delete the starting code and paste the entire `code.c` file.</p>

<p><strong>Adjust Memory Settings:</strong></p>
<img width="426" alt="Adjust Memory Settings" src="https://github.com/user-attachments/assets/83eaef1d-3838-4ca2-962a-0ed6e650100b">
<p>In the left sidebar, scroll down and unselect "Memory: Suspicious use of cache bypass". This will ensure the hardware devices connected to the FPGA are correctly simulated.</p>

<p><strong>Compile and Load Code:</strong></p>
<img width="338" alt="Compile and Load Code" src="https://github.com/user-attachments/assets/57106fcc-19f8-41be-a51f-e945896d0aef">
<p>Compile and load the code, then click "Continue" to begin the simulation.</p>

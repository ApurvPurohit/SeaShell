[stars-shield]: https://img.shields.io/github/stars/ApurvPurohit/SeaShell.svg?style=for-the-badge
[stars-url]: https://github.com/ApurvPurohit/SeaShell/stargazers
[issues-shield]: https://img.shields.io/github/issues/ApurvPurohit/SeaShell.svg?style=for-the-badge
[issues-url]: https://github.com/ApurvPurohit/SeaShell/issues
[license-shield]: https://img.shields.io/github/license/ApurvPurohit/SeaShell.svg?style=for-the-badge
[license-url]: https://github.com/ApurvPurohit/SeaShell/blob/master/LICENSE

[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]



<br />
<p align="center">
  <a href="https://github.com/ApurvPurohit/SeaShell">
    <img src="images/logo.png" alt="SeaShell" width="150" height="150">
  </a>
  <h1 align="center">SeaShell</h1>

  <p align="center">
    an interactive shell with signal handling capability    
    <br />    
    <a href="https://github.com/ApurvPurohit/SeaShell/issues">Report Bug</a>
    ·
    <a href="https://github.com/ApurvPurohit/SeaShell/issues">Request Feature</a>
  </p>
</p>



<!-- TABLE OF CONTENTS -->
<br />
<details open="open">
  <summary><b>Table of Contents</b></summary>  
  <ol>
    <li><a href="#about-the-project">About The Project</a></li>
    <li><a href="#features">Features</a></li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgements">Acknowledgements</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

[![Product Name Screen Shot][product-screenshot]](https://example.com)

Creating an interactive shell with signal handling capability

Here's why:
* Your time should be focused on creating something amazing. A project that solves a problem and helps others
* You shouldn't be doing the same tasks over and over like creating a README from scratch
* You should element DRY principles to the rest of your life :smile:

Of course, no one template will serve all projects since your needs may be different. So I'll be adding more in the near future. You may also suggest changes by forking this repo and creating a pull request or opening an issue. Thanks to all the people have have contributed to expanding this template!

A list of commonly used resources that I find helpful are listed in the acknowledgements.

<!-- FEATURES -->
### Features
The following commands should be supported.

1.  When the user types command "HISTORY BRIEF", the program should list out all the commands (no need for file names) that were executed so far without the respective arguments. The display should show one command per line.

2. When the user types command "HISTORY FULL", the program should list all the commands so far with their respective arguments. Each command should have an INDEX number associated with it and it should be displayed as well.

3. If the user types "EXEC <COMMAND_NAME>", your program should execute that command with its arguments.

4. When  If the user types "EXEC <COMMAND_INDEX_NUMBER>", your program should execute that command with its arguments.

5. The program should exit when the user types a special command "STOP".

6. The program should handle following border conditions:
a. At least one file name should be supplied when the program starts.
b. For unrecognized commands (not in history), the program should type appropriate message.
c. Avoid memory leaks and segmentation faults. Don't allocate memory where not required and as far as possible free memory.

<user@iitjammu>./a.out <file-name1> <file-name2>
This section should list any major frameworks that you built your project using. Leave any add-ons/plugins for the acknowledgements section. Here are a few examples.
* In this lab task, unix type shell which will read commands from the user and execute them appropriately. There are two types of commands that can be expected from the user : a user-defined command and a system-command. The following are the specifications for the project. For each of the requirement the appropriate example is given along with it.

Specification -1 : Display requirement

When you execute your code a shell prompt of the following form must appear:

<username@system_name:curr_dir>.

E.g., <daksh@iitjammu:~>

The directory from which the shell is invoked will be the home directory of the shell and should be indicated by "~"

If the user executes "cd" change dir then the corresponding change must be reflected in the shell as well.

E.g., ./a.out
<daksh@iitjammu:~>cd newdir
<daksh@iitjammu:~/newdir>

Specification -2 :  User-defined commands

The following commands must be supported by the shell
-pid : prints the process id of your shell program

E.g., 
<daksh@iitjammu:~>pid
command name: ./a.out  process id: 234

-pid current : prints the list of the process ids of the processes that are created by the shell and currently active
<daksh@iitjammu:~>pid current
List of currently executing processes spawned from this shell:
command name: emacs   process id: 235
command name: xbiff   process id: 448
command name: xcalc   process id: 459

-pid all : prints the list of process ids of all commands that were executed so far by the shell (including the currently executing processes)

E.g.,
<daksh@iitjammu:~>pid current
List of all processes spawned from this shell:
command name: ls      process id : 112
command name: cd      process id : 124
command name: emacs   process id : 235
command name: xbiff   process id : 448
command name: xcalc   process id : 459



-HISTN: list of last "N" commands executed by the shell. If the number of commands is less than "n" then the shell should print only the available number of commands. 

E.g.,
<daksh@iitjammu:~>HIST5 (Say print last 5 commands assuming the above history of commands)

1. emacs
2. xbiff
3. xcalc
4. vi
5. ps


-!HISTN : execute history command number "n" (assuming the first command is numbered 1)

E.g.,
<daksh@iitjammu:~>!HIST4 (assuming above history of commands)
vi


-Allowed to retain existing support for EXEC, HISTORY FULL, HISTORY BRIEF


Specification 4: System commands with and without arguments

All other commands are treated as system commands like : ls, emacs, vi and so on. The shell must be able to execute them either in the backgroud or in the foreground.

--Foreground processes: For example, executing a "vi" command in the foreground implies that your shell will wait for this process to complete and regain control when this process exits.

--Background processes: Any command invoked with "&" is treated as background command. This implies that your shell will spawn that process and doesn't wait for the process to exit. It will keep taking user commands. If the background process  exits then the shell must display the appropriate message to the user.

Specification -1 : Output redirection with ">"

<daksh@iitjammu:~>wc temp.c > output.txt
<daksh@iitjammu:~>cat output.txt
xx xx xx
<daksh@iitjammu:~>

Specification -2 : Input redirection with "<"

<daksh@iitjammu:~>wc < temp.c 
xx xx xx

Specification -3 : Input/Output redirection with "<" ">"

<daksh@iitjammu:~>wc < temp.c > output.txt
<daksh@iitjammu:~>cat output.txt
xx xx xx

Specification -4: Redirection with pipes (multiple pipes should be supported

<daksh@iitjammu:~>cat temp.c | wc
xx xx xx

Specification -5: Redirection with pipes and ">"

<daksh@iitjammu:~>cat temp.c | wc | output.txt
<daksh@iitjammu:~>cat output.txt
xx xx xx


<!-- HOW TO RUN -->
## Usage

* Clone the Project
* Run the Makefile in Project directory (`make`)
* Run the terminating command _STOP_ to exit SeaShell (`STOP`)

<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to be learn, inspire, and create. Any contributions you make are **greatly appreciated**.
Here's how you can contribute:

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request



<!-- LICENSE -->
## License

Distributed under the MIT License. See [LICENSE](https://github.com/ApurvPurohit/SeaShell/blob/master/LICENSE) for more information.



<!-- CONTACT -->
## Contact

#### Ask me anything [here](https://github.com/ApurvPurohit/ApurvPurohit/issues).


<div>
  <i>Follow me around the web:</i><br />
 <a href="https://twitter.com/ApurvPurohit4">
  <img align="center" alt="Apurv's Twitter" width="44px" src="https://raw.githubusercontent.com/peterthehan/peterthehan/master/assets/twitter.svg" />
</a>&nbsp;
<a href="https://linkedin.com/in/apurvpurohit">
  <img align="center" alt="Apurv's LinkedIn" width="44px" src="https://raw.githubusercontent.com/peterthehan/peterthehan/master/assets/linkedin.svg" />
</a>
  
 </div>


<!-- ACKNOWLEDGEMENTS -->
## Acknowledgements
* [Dr. Bruhadeshwar Bezawada](https://scholar.google.co.in/citations?user=BvG80WkAAAAJ&hl=en)

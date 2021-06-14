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
    A custom Bash written in C    
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
    <li><a href="#contribute">Contribute</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgements">Acknowledgements</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

SeaShell is a custom Unix-type shell written in C. It emulates basic Bash specifications like batch-processing(shell scripts), piping, signal-handling, I/O redirection, etc and the history feature inspired from ***ksh*** and ***csh***. Read more about Bash [here](https://en.wikipedia.org/wiki/Bash_(Unix_shell)).

<!-- FEATURES -->
## Features


### ***Display***

Display Format: `<username@system_name:curr_dir>`

The directory from which SeaShell is invoked is the home directory, indicated by `~`. If the user executes `cd` (change dir), the corresponding change is reflected in SeaShell as well.

E.g., `>./SeaShell`

`<johndoe@loremipsum:~>cd newdir`

`<johndoe@loremipsum:~/newdir>`


### ***Batch-processing of Commands: SeaShell Scripting***

1. SeaShell supports processing of multiple batch files. They should be provided as space-seperated arguments while running the executable as: 

  `./SeaShell <file-name1> <file-name2>`

2. After processing of batch files (if supplied), SeaShell goes to a post-processing interactive mode, similar to a unix-type shell.

3. The batch file can contain user-defined as well as generic unix system commands. [Sample Batch File](https://github.com/ApurvPurohit/SeaShell/blob/master/sample_batch.txt)
 

### ***User-defined Commands***
  
1. `HISTORY BRIEF` - Lists out all the commands that were executed so far without the respective arguments, one command per line.

2. `HISTORY FULL` - Lists out all the commands so far with their respective arguments. Each command has an INDEX number associated with it and it is displayed as well.
  
3. `HISTN` - Lists last "N" commands executed by SeaShell. If the number of commands is less than "N", it prints only the available number of commands. 

4. `!HISTN` - Executes HISTORY command number "N" (assuming 1-indexing).

5. `EXEC <COMMAND_NAME>` - Executes the user supplied command with its arguments.

6. `EXEC <COMMAND_INDEX_NUMBER>` - Executes the command at user supplied index in HISTORY with its arguments.

7. `STOP` - Terminates SeaShell.

8. `pid` - Prints the process id of the SeaShell process.

9. `pid current` - Lists the process ids of the processes that were spawned by SeaShell and are currently active.

10. `pid all` - Lists the process ids of all commands that were executed so far by SeaShell (including the currently executing processes).

(*All other commands are treated as system commands like ls, emacs, vi and so on.*)


### ***Context-specific Execution***

1. **Foreground processes**: Executing any command normally(in the foreground) implies that SeaShell will wait for the process to complete and regain control when the process exits.

2. **Background processes**: Any command invoked with a `&` argument is treated as a background command. This implies that SeaShell will spawn the process and proceed(won't wait for the process to exit). When the background process exits, an appropriate indication message is shown to the user.


### ***I/O Redirection***

1. Output redirection with `>`

`<johndoe@loremipsum:~>wc temp.c > output.txt`

`<johndoe@loremipsum:~>cat output.txt`

`xx xx xx`

2. Input redirection with `<`

`<johndoe@loremipsum:~>wc < temp.c`

`xx xx xx`

3. Input/Output redirection with `<`, `>`

`<johndoe@loremipsum:~>wc < temp.c > output.txt`

`<johndoe@loremipsum:~>cat output.txt`

`xx xx xx`

4. Redirection with pipes (multiple pipes supported)

`<johndoe@loremipsum:~>cat temp.c | wc | output.txt`

`xx xx xx`

5. Redirection with pipes and `>`

`<johndoe@loremipsum:~>temp.c | wc > output.txt`

`<johndoe@loremipsum:~>cat output.txt`

`xx xx xx`


<!-- HOW TO RUN -->
## Usage

* Clone the repository and `cd` to repository root.
* Run the Makefile(`make`).
* Run the executable with/without batch file arguments (`./SeaShell`).

<!-- CONTRIBUTING -->
## Contribute

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

IL2212 - Embedded Software Lab Project
======================================

This document presents a list of guidelines for a possible workflow in the IL2212 lab course. The scope of this lab is to:

* familiarize students with industrial tools and automatic design flows by opting for a free and open hands-on approach;
* enforce collaborative development through management tools like code versioning and issue tracking;
* challenge the design skills by understanding the system requirements provided in a formal language and requiring optimized implementation solutions.

By following these simple guidelines we are trying to ensure a smooth and efficient development flow not just for for the lab tasks, but for the lab itself. One must understand that these tools are far from "perfect" and the lab itself is a "playground" being continuously enhanced, but it is our duty as _engineers_ to ensure that everything works in a satisfactory manner and that the requirements are delivered efficiently. We also encourage any development effort and idea for raising the quality of education within this course.

Before starting to work on the lab assignments, we recommend reading this guide and the suggested documentation, and getting familiar with the "Git" versioning system. We also recommend playing as much as possible with concepts like _forking_, _branching_, _merging_, _issue tracking_, until you feel that you mastered them. You should go through [this manual](https://git-scm.com/documentation) (especially the introductory chapters) and keep it as a reference at any time. Another good resource to go through is [this tutorial](https://www.tutorialspoint.com/git/git_quick_guide.htm).

Also, if you haven't done so already, please take your time to familiarize with the provided Linux environment. It is up to you to choose a work style or which programs/IDEs to use. Mind though that support and documentation will only be provided for the command line environment, so you might want to play around with the terminal. Here is a [tutorial on how to use basic terminal commands](https://www.youtube.com/watch?v=IVquJh3DXUA) and a [tutorial on how to install programs using apt-get](https://www.youtube.com/watch?v=EKmLXiA4zaQ).

## Basic setup

Make sure you each have an account on [gits-15.sys.kth.se](gits-15.sys.kth.se) (refer to the [KTH GitHub](https://www.kth.se/en/student/kth-it-support/work-online/kth-github/kth-github-1.500062) page).

1. The first thing that you need to do is to [set up Git on your virtual machine](https://help.github.com/enterprise/2.10/user/articles/set-up-git/#setting-up-git) using your KTH ID and credentials. This guide assumes you have set up your virtual machine according to the **Connecting over SSH** part and shall use the SSH URL as reference for the main (public) repository. Instructions can be found also on [KTH GitHub page](https://www.kth.se/student/kth-it-support/work-online/kth-github/github-read-me-first-1.500157).

   * If you use the virtual machine `git` is already installed. (Otherwise, you can install git from this [site](http://git-scm.com/).)
   * You need to generate _ssh keys_ and add them your `gits-15` account in order to gain access to the git repository. Please follow [this instruction](https://help.github.com/articles/generating-an-ssh-key/). (When you run the test in Step 5, give the name of the KTH server, i.e. `ssh -T git@gits-15.sys.kth.se`. Not _git@github.com_.) The procedure goes along the lines of:
        
          sudo apt-get install git
          git config --global user.name "name"
          git config --global user.email "name@kth.se"
          
          # check if existing generated keys
          ls -al ~/.ssh
          
          # generate ssh key if none present 
          ssh-keygen -t rsa -b 4096 -C "name@kth.se"
          
          # start the ssh-agent in the background and add the key
          eval "$(ssh-agent -s)"
          ssh-add ~/.ssh/id_rsa
          
          # copy the generated key found below to your GitHub account
          emacs ~/.ssh/id_rsa.pub
   * You can also install any GUI (`git-gui` and `gitk` are already there), but this guide will only use the command line interface. 
    
1. Each group of students registered in Canvas will receive an invitation from the teaching staff to be collaborator to a Git repository named along the lines of `il2212_surname1_surname2`. Once you accept the invitation, you need to:

   * Clone the newly created repository on your local machine:

          git clone <your_repository_ssh_url>
  
   * [Add a remote to the main (public) repository](https://git-scm.com/book/en/v2/Git-Basics-Working-with-Remotes). Assuming that you have set up SSH authentication:

          cd <path/to/your/il2212_surname1_surname2>
          git remote add public git@gits-15.sys.kth.se:ugeorge/il2212-project.git
          git pull public master
          git push origin
    
    - you now have a local copy with two remotes: `origin` is the remote to your private repository where you will commit your changes to; `public` is the remote to the original repository (`ugeorge/il2212-project`) where you get all the updates from.

1. Make sure you are [watching](https://help.github.com/enterprise/2.3/user/articles/be-social/) the public repository, so that you are notified in the GitHub news feed whenever a change has been made. In case this occurs, you must [merge](https://git-scm.com/book/en/v2/Git-Branching-Basic-Branching-and-Merging) all the public changes into your local work flow, e.g.:

    ```
    git pull public master
    ```
    
    It might be so that you need to manually manage conflicts in case the automatic merge fails. Refer to the provided documentation on methods and tools for merging code. Take your time in the beginning until you understand how to properly integrate new code, and make sure you test your project after each merge. In case a serious mistake is done, you can always revert to the HEAD or earlier revisions.


## Work flow

1. Start playing with your git tools as soon as possible. Here is a [cheat sheet](https://education.github.com/git-cheat-sheet-education.pdf) with the most important commands. The basic commands for committing your changes are:
    ```
    git add <file_or_folder> # git add -A in case there are new unversioned files
    git commit -m "<message>"
    git push origin <your_branch> 
    ```

1. When committing your work, it is important to tag each commit with a meaningful message, so that all contributors understand what changes have been made and it is easier to roll-back to earlier revisions in case a mistake has been made. Here is a guide on [how to write better commit messages](https://robots.thoughtbot.com/5-useful-tips-for-a-better-commit-message), so that your repository doesn't end up [like this](https://xkcd.com/1296/).

1. Make use of the [`.gitignore`](http://git-scm.com/docs/gitignore) file. You should never commit intermediate or generated files. The repository should contain only source code and build/automation scripts.

1. Make a habit of branching your work whenever you implement or experiment with new features. Apart from that, each student in a group should work on her/his own separate branch on different work sets. The `master` branch should contain _final_ or _working_ features. Refer to [this document](https://git-scm.com/book/en/v2/Git-Branching-Basic-Branching-and-Merging) for an overview on the basic principles of branching and merging. 

**OBS1:** It is up to the students to distribute and manage work packages within the group. Keep in mind that individual contribution will be transparent and is expected to be as balanced as possible. Evaluation for the laboratory course is given for _individual_ contribution, as well as the project as a whole.

**OBS2:** Prior to each lab session the course staff shall compile and run the projects on the `master` branch. This means that it is the students' duty to provide correct source code and automation scripts so that the results can immediately be verified against the reports. Failure to do so, as well as inconsistencies within the results means that you cannot pass the lab course. 

**OBS3:** Your code _will_ be verified against plagiarism. We do recommend open discussions among yourselves and sharing of ideas, but _we will not accept cheating and (even partially) copied code_!

## Directory structure

This repository is structured as follows:
 * `spec-model` contains the executable specification of the application you need to implement for the project. It is provided as a ForSyDe-Haskell project which implements a network of SDF processes, capturing the functional and extra-functional specifications. You need to install, run and understand this model. Refer to the [README](spec-model/README.md) file for more information.
 * `app` contains the soure code for the project. Here you can find the _hello world_ examples and our provided code snippets. Here is also where you shall implement your lab projects. It is advisable to create new folders for each new project, otherwise you will have to manage merge conflicts with each update of the main repository. Also, if you intend to reuse the provided build scripts, you should keep the directory structure suggested by the demo applications.
 * `bsp` is where the the "board support package" (libraries, device drivers, etc.) will be generated. Unless you have good reasons, you should leave it untouched.
 * `hardware` is where the architecture/hardware files reside. You should check it out, but for this lab you are not supposed to modify anything.

## Issues. Contributions

Any bug, feature or problem can be managed within a lab group using [the issue tracker](https://guides.github.com/features/issues/). We recommend using it freely on the local repository as means to manage work packages and communicate problems. Please avoid tagging course staff unless it is a _very important_ and _relevant_ topic. **OBS:** any coding problem or bug will be solved _solely_ by the students, and the course staff will _not_ debug your code. 

If you identify a problem of general interest or related to the provided material you can open an issue on the main (public) repository, which will be visible to all course attendants. Please take your time and be mindful of what you post in the public domain since it is _really important_ that the issue is correct, concise, and is accompanied by a short description of context in which the issue arose. [Here](https://wiredcraft.com/blog/how-we-write-our-github-issues/) and [here](https://upthemes.com/blog/2014/02/writing-useful-github-issues/) are two discussions in how to write meaningful issues. An issue can be confirmed or invalidated, depending on whether or not it is reproducible.

If you know how to solve an opened issue and would like to contribute to the development of the lab, feel free to [fork](https://help.github.com/enterprise/2.4/user/articles/fork-a-repo/) the main repository and [issue a pull request](https://help.github.com/articles/using-pull-requests/) when you believe you have solved the problem. The solution will be evaluated and if valid, will be merged into the `master` branch. **OBS:** _DO NOT COMMIT LAB CODE IN YOU FORKED REPO!_. It is public, and everybody can see it. Use the provided "hello world" examples, or simply do not commit your own code. We shall take drastic measures if any solution is published without the course staff's consensus.

## Communication with the lab staff

Here are some guidelines to ensure an efficient communication with the staff concerning the lab project:
 * use the issue tracker to manage and track your development between group members. If an _important_ issue involves the lab staff, you can either tag staff member(s) (`@<user_name>`) in the issue message, or include them in the assignee list. Remember that the asnwer might arrive very late, so please use this feature only when it is really necessary, or when you are instructed to do so.
 * the lab staff will not debug your code or take care of the issues. It is your duty to do that.
 * in case your issue concerns all students, you are advised to post your question on the Canvas discussion page, where it is more likely to get a fast answer from your peers.
 * the lab staff can assign issues to your group in the following cases:
   * during the lab session, based on the outcome of checking the lab. You will be asked to write the issues on your own laptop, and announce the staff after solving it via the issue tracker.
   * after the issue has been checked, if the solution is unsatisfactory. 
 * **OBS**: you are not supposed to close an issue opened by the lab staff. The staff will close the issue after checking if the solution is correct.

## Summary

Here is a summary of the suggested workflow:
 * You create and clone a private repo. This is where you commit your work. The master branch shall contain your project in a compilable and runnable state.
 * The lab results (along with the other requirements) shall be present in the lab report which is submitted to Canvas as an assignment. They will be compared against the results given by the program's runtime.
 * Your local clone shall have a remote to the current public repository (git@gits-15.sys.kth.se:ugeorge/il2212-project.git), from where you have to (regularly) pull updates.
 * You are advised to use separate branches for different work packages which are regularly merged into the master branch. You are also advised to take advantage of the issue tracker to micromanage yourselves your work packages.
 * If you are assigned a specific task by the lab staff, you need to take care of it and announce (via tags or issue re-assignment) once it is fixed. 

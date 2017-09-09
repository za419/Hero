# Contribution Guidelines

First of all, I would like to welcome you to VersionControl. This is just a personal
pet project, and I am hard-pressed to imagine it becoming a real competitor to
something like Git, but I am happy to have you help me see what we can make it into.

## Table of Contents

 - [Developer Resources](#developer-resources)

   - [Issue tracking](#issue-tracking)

   - [Documentation](#documentation)

   - [Communication](#communication)

 - [Testing](#testing)

 - [Development environment](#development-environment)

 - [Change submission](#change-submission)

 - [Bug reporting](#bug-reporting)

 - [Feature requests](#feature-requests)

 - [Roadmap](#roadmap)

 - [First bugs for contributors](#first-bugs-for-contributors)

 - [Style guide and Conventions](#style-guide-and-conventions)

 - [Code of Conduct](#code-of-conduct)

 - [Project management](#project-management)

   - [Asking a question](#asking-a-question)

 - [Closing thoughts](#closing-thoughts)

-----

## Developer Resources

Because VersionControl is currently a very small project, there isn't a great deal
in the way of developer resources. If you wish to reach me, submitting an issue will
normally be sufficient. However, if the need to contact me is urgent, I can be
reached at my personal email (ryan951357@gmail.com). Please use this sparingly: If
you use it too much, it may interfere in the normal use of my email, and I may be
forced to block addresses to prevent this.

### Issue tracking

Issue tracking is done entirely on the [main GitHub
repository](https://github.com/za419/VersionControl/issues). Of course, forks of the
project may maintain separate issue trackers, but I will generally not work to fix
an issue unless it appears in the tracker in my repository.

Issues are marked with as much relevant information as possible (in terms of
milestones, labels, etc.), which includes priority labels. There are four priority
labels, which are (in order):

1. CRITICAL

2. High Priority

3. Medium Priority

4. Low Priority

When choosing an issue to fix, you should assign yourself (or leave a comment
claiming the issue) to the issue before you begin work. Don't do so too long
beforehand, as someone else may be discouraged by your assignation, but try not to
start working on the issue before claiming it.

As a general rule, you should select the highest priority issue you think you can
handle: If there are multiple such issues, then you can choose one at your
discretion. As a rule of thumb, I recommend either selecting the one you think is
'easier' (especially for higher priority) or the one which is older.

Some issues have the 'help wanted' tag, which generally means that they're looking
for someone to fix them. If you're up to that task, assign yourself and remove the
label, or leave a comment about it.

### Documentation

Currently, all documentation is written as comments in the code files. While I have
made an effort to keep these sufficient to inform contributors, I realize that this
is not really sufficient. I therefore intend to add documentation on the project
wiki on GitHub (see issue #3).

### Communication

Currently, there isn't a formal communications structure for this project, so
conversation will probably be taking place on GitHub. I am not, however, opposed
to creating a better platform for this, and I intend to do so if the population of
contributors becomes cumbersome for the project.

## Testing

At the moment, testing is done manually and informally, following no rigid
structure, and simply executing commands on a mock repository. Obviously, this is
insufficient, so a test suite shall be written sometime hopefully soon (see issue
#4).

## Development environment

While the code in this repository is constructed with non-Windows support in mind,
there is currently no authoritative environment for development on these
platforms, and the code has not been tested there (although, via
`crossplatform.h`, it should at least compile). A pull request adding necessary
changes for *nix development (such as a Makefile) would be welcomed.

On Windows, the authoritative developer environment is Visual Studio Community
2017, which can be downloaded from [this
page](https://www.visualstudio.com/downloads/). Earlier versions will almost
certainly not work. Later versions should, but if a pull request causes the project
to not be openable on VS2017, it will be rejected. Alternative editions of VS2017
should work, but again, if a change is incompatible with Community, it is
unacceptable.

## Change submission

All changes should be submitted as pull requests on the GitHub page. I will do my
best to look at a request on the same day it is entered.

If I have questions, I will leave them as comments on the request. If I reject the
request, I'll leave a comment explaining why and close the request: After fixing
the problems, you may resubmit your changes.

If your changes are too complicated to make a decision immediately, I'll leave a
comment saying something to the effect of "Thank you for your request, I'll take a
look at it and get back to you shortly." before I begin evaluating the changes.

Finally, on occasion, your request won't be looked at for a few days. Sometimes,
people are busy, we all get occupied at times, and it can be difficult to stay
up-to-date with side projects. If you submit a pull request, and it takes longer
than a week to receive a reply, send me an email.

## Bug reporting

All programs suffer bugs on occasion, and I would be astounded if I discovered
this to be the exception.

Bug reporting should be done by reporting an issue on GitHub. Title the issue with
[BUG], followed by a succint description of the problem. In the issue description,
add any information you have on the issue, which includes but is not limited to:

 - Recommended priority (critical, high, medium, low)

 - Steps required for reproduction

   - If applicable, including which environment this occurs on

 - Consistency of reproduction (does this always happen?)

 - Expanded description of the probelm

## Feature requests

Features should always be prioritized after bugfixes, and will be labeled in the
issue tracker with "enhancement".

Requesting a feature should also be done through the GitHub issue tracker. Make a
new issue, titled with [FEATURE], followed by a succint description of the feature
you're requesting. In the issue description, add a more in depth description of
the feature you're requesting, which includes all of:

 - Recommended priority (critical, high, medium, low) - Be conservative!

 - What behavior the feature should have

 - How this behavior should be invoked, in your opinion

 - Justification for why the feature should be included

   - If, in your opinion, this is obvious, you should still include it.

## Roadmap

This section details, in fairly general turns, my plans for expanding the project.

At the time of writing, version 0.01 has been released. This fits the following
requirements:

 - [x] Can initialize a repository

 - [x] Can add files to index (for later commit)

 - [x] Can create a commit from files in the index

There are only two features which are documented, but not implemented in version
0.01:

 - [ ] Can use `commit -a` to automatically add files that were in the last
commit to the commit being made

 - [ ] Can use `commit` with a list of files to automatically add these files
and commit

These features, as they are noted in the usage information but not actually
implemented, should be the next ones to be implemented, as soon as feasible.

Along with these two features, the addition of two commands is intended for
version 0.02:

 - [ ] `log` will, starting at the HEAD, display all commits back to the initial
commit

 - [ ] `checkout` will, given a complete hash, or 'HEAD', overwrite the contents
of applicable files with the versions in the given commit

   - If this is given a commit besides the HEAD, either the implementation
should take steps to either:

     1. Prevent a new commit from being made

     2. Warn the user about making a commit

     3. Allow commits to be made, but off of the checked out commit (probably
the correct option, in conjunction with 2, once branches are implemented)

   - If this is given HEAD, it should remove that protection

   - Checkout should confirm that what it checks out matches the committed
SHA256, and issue a warning if there is a mismatch. This is not necessarily an
error condition, but its worth warning about.

Version 0.03 will include one major change:

 - [ ] File operations will support directories

But may also include minor functionality upgrades to such components as argument
parsing.

Version 0.04 will add support for:

 - [ ] Branches

   - `checkout` will support being given the name of a branch, and will checkout
that branch head

   - Merging will be possible, following the following semantics:

     - If a file exists in one branch but not the other, the existing branch
will supply the file

     - If both branches have a copy of the file, but both versions have the same
checksum, the first mentioned branch will supply the file

     - If both branches have different copies of the file, but one is from a
parent commit of the one containing the other, the branch with the latest
version will supply the file

       - The way this would currently have to be done (parsing every single
commit in both branches until a commit is found that doesn't have a match) will
be very slow. Perhaps `merge` should accept an argument to skip this check?

     - If both branches have diverging copies of the file, then the user will be
prompted to select which branch will supply the file.

   - Note that merge doesn't have a way to merge the contents of files. That
comes later.

   - Merge will produce a merge commit, with two parents, being the heads of
parent branches.

   - As far as `log` is concerned, branches don't exist, at least after we
select a head commit.

 - [ ] Stash

   - Commits can be placed in a special 'stash' branch, which doesn't track
history, just preserves work for later.

Version 0.05 will support:

 - [ ] Three-way merge

   - Given two branches, the merger will determine the last common ancestor, and
perform a standard three way merge on these commits

   - The existing merge algorithm will continue to be supported, as an argument
tomerge`

 - [ ] Producing a diff between any two of:

   - HEAD

   - A branch

   - A commit, specified by hash

   - The contents of the index

   - The working directory

 - [ ] `commit --amend`

   - Allows the user to edit the last commit title and message, updating the
date and time to current

Version 0.1a will support:

 - [ ] Diff-encoded commits

   - Every X commits is a 'principal commit', encoded with all files

     - This can be edited by repacking the repository (see below)

   - Every other commit is encoded as the patch required to transform the last
commit's files into the new commit.

   - Merge commits are always principal commits, regardless of if its their turn 
(to avoid diff bugs)

     - If X is set to -1 (all diff encoded, or only principal is the initial), 
then each file in a merge commit will specify which parent is the diff base 
for that file

 - [ ] `gc`

   - Confirms that the repository is encoded properly

   - Deletes all commits that cannot be accessed from the set of all branch heads

 - [ ] `repack`

   - Alters the settings for diff encoded

   - Can set X (above) to 0 for all-principal encoding (legacy), or -1 for all
diff encoded (minimum space consumed)

 - [ ] `fsck`

   - Checks all reachable commits, and checks whether their hash matches their
filename.

   - Reports all inconsistencies to the user

 - [ ] diff-encoded stashing

   - Stashes are now patches from the head commit at the time they were stashed,
and can be applied onto the current head commit.

Version 0.1a will be the first alpha testing version. Versions thereafter are
unplanned for now. However, the following features are in consideration for 
development following the beginning of alpha testing:

 - A decoded-files cache, which holds copies of un-diffed files from various 
commits, separately from the commit data.
 
   - Tools would first check for the version of the file they're looking for in 
this cache:
   
     - If a file is there, use it

     - If it is empty, fill it after the file is created for the tool's use

     - If it is not present, but a version of the file within a few commits is, do 
nothing.

     - Else, create an empty file

   - This cache would be cleared by `gc`

   - The advantage of this would be a negation of the speed downside to the diff 
encode, without the increased size of commits

     - However, the cache will still consume a large amount of disk space. The 
filling of the cache is conservative, so it should only involve commonly accessed 
files, but of course there will be a tendency for the cache to be filled with 
files the user never sees, but are used to decode descendant versions.

       - Perhaps some tools should not fill the cache, or the fill should be 
restricted to files in recent commits only.

 - Automatic prompting to run `gc` every so often
 
   - This is especially needed for the cache, since its size will balloon if it is 
not commonly cleared, and we don't expect users to run `gc` often.

 - Provide a way for users to format `log` output
 
 - Rebasing
 
   - This is pretty complicated work, so don't expect it anytime soon.

## First bugs for contributors

The easiest issues to fix will often be those related to documentation - Be
careful, however, to understand what you're writing documentation for.

With regards to the code, the easiest bugs to fix will generally be those
regarding the handling of information provided in well-defined formats. Examples
include command-line arguments, or handling well-formed commits.

## Style guide and Conventions

Generally, the style is as formatted by Visual Studio by default. Opening braces
are usually left on the same line as their conditional, blocks of code are
indented relative to their surroundings, and trailing whitespace is cleared. To
facilitate compliance with this, I use [Power Commands for Visual Studio](https://marketplace.visualstudio.com/items?itemName=VisualStudioProductTeam.PowerCommandsforVisualStudio),
and enable the 'Format document on save' option.

In general, try to comply with the style of surrounding code. Unless your code is
styled completely horribly, I will not reject it on a stylistic basis: However,
note that if I am on the fence whether a request should be merged, I will take
readability and ease of maintenance into account in my decision.

The most important code convention is that any set of code which will be run more
than once in a program, in the same way, should be a function. Functions, in
general, should be in separate files: At the moment, this follows a
header-file-only design, where functions are defined entirely in headers, and
there is a single object file containing `main`.

Examples of these headers are:

 - `Utils.h`

   - Small functions which perform relatively trivial tasks - In general, these
should be small enough to be inlined.

 - `crossplatform.h`

   - Functions which, using the preprocessor, encapsulate tasks which are
implemented differently on different platforms.

Other conventions should be noted here as they are developed.

## Code of Conduct

See the [Code of Conduct](../CODE_OF_CONDUCT.md).

## Project management

At the moment, I am the only developer of this project, and I therefore claim sole
responsibility for managing it. If this becomes too difficult, I will choose
persons who have a history of work and familiarity with the project to assist me,
these persons will be marked as collaborators on GitHub, and, with their
permission, they will be listed here.

Project roles:

 - Ryan Hodin (ryan951357@gmail.com)

   - Project lead, administrator, et cetera.

### Asking a question

I would be happy to help you with any questions you may have. Just submit an issue
on GitHub, whose title starts with [QUESTION], and I'll do my best to help you.

## Closing thoughts

Thank you very much for your interest in helping with this project, and thank you
in advance for any contributions you provide. Your time is very valuable, most so
to yourself, and I thank you very much for your having spent some here.

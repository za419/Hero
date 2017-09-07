# Contributing Guidelines

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
 
 - [First bugs for contributors](#first-bugs-for-contributors)
 
 - [Style guide and Conventions](#style-guide-and-conventions)
 
 - [Project management](#project-management)
   
   - [Asking a question](#asking-a-question)
   
 - [Closing thoughts](#closing-thoughts)

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

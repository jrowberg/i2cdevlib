Contributing to Mosquitto
=========================

Thank you for your interest in this project.

Project description:
--------------------

The Mosquitto project has been created to provide a light weight, open-source
implementation, of an MQTT broker to allow new, existing, and emerging
applications for Machine-to-Machine (M2M) and Internet of Things (IoT).

- <https://projects.eclipse.org/projects/technology.mosquitto>


Source
------

The Mosquitto code is stored in a git repository. The URL for anonymous access is:

- http://git.eclipse.org/gitroot/mosquitto/org.eclipse.mosquitto.git

Access via Gerrit (for code review) is through the following URLs:

- ssh://username@git.eclipse.org:29418/mosquitto/org.eclipse.mosquitto
- https://username@git.eclipse.org/r/mosquitto/org.eclipse.mosquitto

A web browsable repository is available at

- <http://git.eclipse.org/c/mosquitto/org.eclipse.mosquitto.git>

Contributing a patch
--------------------

The Mosquitto repositories are accessed through Gerrit, the code review
project, which makes it possible for anybody to clone the repository, make
changes and push them back for review and eventual acceptance into the project.

To do this, you must follow a few steps. The first of these are described at

- <https://wiki.eclipse.org/Development_Resources/Contributing_via_Git>

* Sign the Eclipse CLA
* Use a valid commit record, including a signed-off-by entry.

There are further details at

- <https://wiki.eclipse.org/Development_Resources/Handling_Git_Contributions>

If your contribution is a fix for a bug, please use the 'fixes' branch as the
base for your work. If you are proposing new behaviour/features please use the
'develop' branch.

Once the patch is pushed back to Gerrit, the project committers will be
informed and they will undertake a review of the code. The patch may need
modifying for some reason. In order to make amending commits more
straightforward, the steps at
<https://git.eclipse.org/r/Documentation/cmd-hook-commit-msg.html> should be
followed. This automatically inserts a "Change-Id" entry to your commit message
which allows you to amend commits and have Gerrit track them as the same
change.

What happens next depends on the content of the patch. If it is 100% authored
by the contributor and is less than 1000 lines (and meets the needs of the
project), then it can be committed to the main repository. If not, more steps
are required. These are detailed in the legal process poster:

- <http://www.eclipse.org/legal/EclipseLegalProcessPoster.pdf>

Developer resources:
--------------------

More information regarding source code management, builds, coding standards,
and other topics can be found at.

- <https://projects.eclipse.org/projects/technology.mosquitto/developer>


Contributor License Agreement:
------------------------------

Before your contribution can be accepted by the project, you need to create and
electronically sign the Eclipse Foundation Contributor License Agreement (CLA).

- <http://www.eclipse.org/legal/CLA.php>


Contact:
--------

Contact the project developers via the project's "dev" list.

- <https://dev.eclipse.org/mailman/listinfo/mosquitto-dev>


Search for bugs:
----------------

This project uses Bugzilla to track ongoing development and issues.

- <https://bugs.eclipse.org/bugs/buglist.cgi?product=Mosquitto>

Create a new bug:
-----------------

Be sure to search for existing bugs before you create another one. Remember that contributions are always welcome!

- <https://bugs.eclipse.org/bugs/enter_bug.cgi?product=Mosquitto>

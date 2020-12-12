# Using DirtyJTAG with UrJTAG

<p align="center"><a href="http://urjtag.org">Project page</a></p>

UrJTAG is one of the oldest FOSS JTAG software. Even though the project isn't as active as it was years ago, it remains a valuable tool for tinkering with EJTAG boards, or reverse engineering blackbox JTAG devices. Its codebase is relatively easy to understand, making it easy to hack new features.

Mainline UrJTAG has DirtyJTAG support since [763bbc](https://sourceforge.net/p/urjtag/git/ci/763bbce1213f5759e2925773d0dd5f3b537368f6/tree/) revision and 2017.10 release. However, some package sources still use outdated UrJTAG from 2011, so you'll need to compile UrJTAG yourself :

```bash
cd /tmp
git clone https://git.code.sf.net/p/urjtag/git urjtag-git
cd urjtag-git/urjtag/
./autogen.sh
make
sudo make install
```

Once installed, you can use DirtyJTAG like any other JTAG cable in UrJTAG :

```text
$ jtag
UrJTAG 0.10 #
Copyright (C) 2002, 2003 ETC s.r.o.
Copyright (C) 2007, 2008, 2009 Kolja Waschk and the respective authors

UrJTAG is free software, covered by the GNU General Public License, and you are
welcome to change it and/or distribute copies of it under certain conditions.
There is absolutely no warranty for UrJTAG.

warning: UrJTAG may damage your hardware!
Type "quit" to exit, "help" for help.

jtag> cable dirtyjtag
jtag>
```

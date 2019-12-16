# The Updater Toolkit
The Updater Toolkit is a experimental *WIP* toolbox for launch hightly modified updater from Sony.

No release is available for now, the source code is only here for research propose.

Based on the work of [TheoryWrong](https://twitter.com/TheoryWrong), [LightningMods](https://twitter.com/LightningMods_) and [Zecoxao](https://twitter.com/notzecoxao).

```diff
- WARNING WARNING WARNING
- This is a very very very experimental toolbox, be careful with this !
- We are not responsible for any damage on your PS4 !
```

<h3>Credits:</h3>

[jogolden a.k.a xemio](https://twitter.com/goldfitzgerald) for ps4-ksdk and this kernel elf loader: [Repository here](https://github.com/jogolden/ps4-ksdk/).

[FlatZ](https://twitter.com/flat_z) for this base app : [Repository here](https://github.com/flatz/ps4_gl_test)

ChendoChap for the 4.74 port of the ps4-ksdk : [Repository here](https://github.com/ChendoChap/ps4-ksdk/tree/4.74/)

All developers and hacker who have contribute to do this !

Also thanks to tester from [PS Test Software](https://discord.gg/JyFpxQ) for your time !

<br/>
You need to put libc.prx and libSceFios2.prx inside the sce_module folder.

<br/>
<h3>How i can get the orbis_swu.self ?</h3>

You simply need to extract it with the FTP from your PS4. You can after just edit with IDA and a HEX Editor.

You need to make some patch for launch it on your PS4, for now, you need to call the syscall 68 at the begin of the process.
See here for orbis_swu (5.05):

![Start syscall patch 1](https://nsa40.casimages.com/img/2019/12/16/191216041055338701.png "Start syscall patch 1")

You're also need to patch the function sceVideoOutOpen for using the bus MAIN (set rdi [Arg 2] to 0).

![sceVideoOutOpen patch 2](https://nsa40.casimages.com/img/2019/12/16/191216041055427269.png "sceVideoOutOpen patch 2")

After, you need to fake sign it with the make_fself from [FlatZ](https://twitter.com/flat_z).
The AuthID for orbis_swu is : *010000000010003800000000001c004000ff00000000008000000000000000000000000000000000000000c000400040000000000000008000000000000000f00040ffff000000f000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000*

<br/>

<h2>Happy hacking ! ;)</h2>

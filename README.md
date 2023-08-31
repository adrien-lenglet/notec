# notec
NOTE-C, shorter and stronger

## Initial public commentary (31st of August, 2023)

This is a long story, maybe the longest one I could tell. Basically, I first got into C programming by being frustrated by how slow BASIC was on my Casio scientific calculator (many years ago, much before even the first commit of this repo). I went through a lot of experiments of all kinds, on C desktop programming, a lot of hacking around. Couple-level full-feature 2D platformer, nice huge entity count 2D engine with many layers, graphic/CLI tooling for these projects.. At some point I even had a desktop PC level editor with Super Mario Bros. 3 sprites and appropriate collision geometry, all hand-designed levels bundled under a single proprietary (of my own!) binary file and rendering on my calculator (C89 renderer). I also did make an assembler and linker that was able to print "Hello world!" on screen but not much more than that before crashing. I did many other fun desktop PC projects in C, like a Osu! bot and a CS:GO stats aggregator with web-scraping in plain C (you heard that right). On each project I loved to get creative and add little easter eggs and entertaining stuff on screen, even if it was unecessary I was learning many new patterns and techniques.

All of these years of fumbling around, discovering low-level programming were a fun & unexpected ride throughout. It's even a bit emotional for me to remember how this long journey unfolded. I think it lasted anywhere from two to three years for me to really get it. This repo is what I'm taking away from all of this.

Even before getting into from scratch programming I loved to mess with commercial engines such as Unity when I was a kid. My first programming language was actually C# and not BASIC. I did two revisions of a game named Paulo, a first in 2011 and a second in 2014, with 30-ish minutes of pretty fun gameplay for each (well, fun especially for the second). I would like to get these released for fun but it's really raw and completely unlicenced game design, so I need to think about how to get these presented.

Anyway, NOTE-C was in continuation of the assembler and linker mentionned above. Both of these were actually on the calculator itself fitting into the couple of MB flash storage along with the necessary runtime static libraries. It included a text editor which I was happy about. But writing applications in assembly was not really reasonable and required a lot of effort, so I wanted to have a C compiler instead of this RISC assembler. Programmers on Casio community forums have told me it was not possible as GCC could not even fit in the entire storage flash memory, but I knew that with stripping enough non-necessary features (not of the language, of the compiler), it could fit in flash and RAM. So I started writing NOTE-C, in C89.

NOTE-C prove difficult to carry through, in part because C itself is not such a powerful language (the absence of templates is tedious), in other part because I was using Hitachi decades-old Windows toolchain, which is really unpleasant to work with. The project dragged out for couple of years before I actually came to the conclusion that, at this point, I just knew better and should use modern C++ on a GCC-based community toolchain.

So it's what I did, and that's when this `notec` repository came into life. This time, I wanted to build a yet-to-defined subset of C++ with at least classes, templates and constant expressions. Because of the 64KiB base RAM, extensible to 256KiB with some tricks on certain models, my immediate concern was for the RAM usage of the overhead related to parsing and storing large headers in memory. So I really designed around compact memory representation for everything, and came to the concept of sequences. See my later published (and now outdated) opinion piece on my website: https://adrien-lenglet.fr/project/spp

At some point I got a parser for prototypes, classes, structs and templates without yet parsing actual expressions. When parsing all of that and storing it, I realized how flawed C/C++ syntax is and made unecessarily difficult to parse. The suggestion of just making my own low-overhead derivated language was growing in my mind, until the point where I just came to stop working on `notec` and go develop that language. I chose to call it S++.

S++ has been in development hell since the last commit of `notec`, but I very recently figured out a lots of aspects of the design of the language while learning in depth about bootloaders and Operating Systems. This brand new effort is in active development and at https://github.com/FMC64/spp !

As for `notec`, it may not immediately come to life. If anything, `S++` will be ported to Casio calculators at some point and then perhaps the `C++` compiler written in `S++`. Stay tuned!
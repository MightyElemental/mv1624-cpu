# MV1624
## A custom 16-bit CPU with vector processing

### Background
After completing a module on CPU design at university, I decided to design my own processor. The processor we used as a template was also 16-bit but had multiple limitations. One of which being the lack of multi-word instructions. This meant an instruction could not access the a full 16-bit memory address and instead was restricted to a 12-bit address. In practice, this was enough to complete the tasks set for our exams, but this limitation meant that only 4KiB of memory was addressable from an instruction directly. The CPU did have indirect memory addressing via the 16-bit registers, but I am unhappy with this workaround for my own processor.

### Overview

#### Experimental Stage

The most significant feature so far is the vector processing unit which constist of four vector registers of four 16-bit values each. Instructions for the vectors will include basic arithmetic, but will also include more complex algorithms to be able to compute useful calculations in fewer clock cycles.
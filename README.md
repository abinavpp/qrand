# qrand

qrand generates random unique questions from a qbank file written in
qrand_format, where the qrand_format is a series of questions in `<q[.\n]*q>` where the part within `<q` and `q>` contains the question. qrand can skip questions from previous runs by feeding it with a qskip file (made during the previous run) and the skip constraint can be relaxed as well. 

To build qrand, run 
`make`

For usage, run
`qrand -h`

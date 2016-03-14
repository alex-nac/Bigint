# Bigint 
Simple c++ big integer class with two optimizations.

1) Copy on write vector: we copy vector of numbers only if we change our big integer.

2) Small object optimization: for big integers which fit <code>size_t</code> range we use <code>size_t</code>. For memory efficiency we use <code>union<code>.


## Plans for the Future

* Random sampling to estimate the quality of a guess, instead of complete search.
* Random guessing when the search space is too large.
* Get rid of `unordered_set` in `Solver`
  * Use a sorted vector of possibilities and binary search for containment.
  
    (Wait a minute... I'm not sure that this is clear cut. Unordered set should be 
    hashing, so O(1) lookups if there are not many collisions. Ordered set
    should be using a binary tree, so O(log n) lookups.)
* Why is startup so slow??


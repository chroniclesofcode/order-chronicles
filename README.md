# order-chronicles
A fast limit order book for trade simulations and performance testing.

DONE:
- Created Matching Engine for orderbook
- Can read in LOBSTER csv data and parse into Orders
- Can process event types of LOBSTER entries properly
- RB + doubly linked lists for orderbook structure
- Can execute trades based on price-time priority
- Can populate the orderbook based on first entry in LOBSTER orderbook file
- Can print orderbook in LOBSTER format to compare to provided orderbook correctly
- Can execute trades based on LOBSTER trade specifications
- Pipes performance measurements into certain files
- Measure performance of the different data implementations
- Provide multiple runthroughs + statistics of each run
- Create maplist-based orderbook (map -> doubly linked lists (custom))
- Implement Arrays-based orderbook of vectors

TO DO:

- Profile hot functions in orderbook
- Optimize parsing of orderbook
- Create the orderbooks with/without STL
- Consider Van Emde Boas Tree (instead of RB) for LogLogN speed
- Update README for detailed information, including tradeoffs and results

FIXES: 
- Patch bug with full cancellation not working properly when orders have been executed on it
- Fix error where order type 4 matches improperly
- Process LOBSTER order book data properly without fail
- Revamp array-based orderbook due to price points being too large
- Fix error in arrays implementation where certain orders are not being added
- Fixed major bug in linear book where orders are randomly changing

# How to run

Currently, this orderbook works with command line arguments, and outputs to stdout. An example of a command may be
```
cmake .. && make && ./main "../assets/AAPL_msgs.csv" "../assets/AAPL_book.csv" > ../assets/AAPL_out.csv
```
For AAPL orderbook processing. This can be applied with other book/msgs if you change the filenames.
Note: this command MUST be run in the 'build' directory. If not, you may alter files outside of
the order-chronicles folder.

# Discussion

Fastest is linear data structure, middle is the default implementation, last is maplist.
There's a lot to talk about here, but I'll keep it short and simple. 

I did believe linear would be
the fastest despite having the poorest time complexity because linear data structures are very easy
for the compiler/cpu to optimize for. Our data structure is a linked list of price levels + vectors
for the orders. Iterating through the orders is very efficient, and I suspect the speed difference
would be even more pronounced in a tight orderbook where price levels have many orders within them.
The inefficiences mostly come from the linked list of price levels - since these levels change so
often, I have to do many removals of linkedlists/sets etc, which definitely is the bottleneck.
I could use a vector for the price levels, but this would probably be another entirely different
implementation since I would need to change all the data structures drastically. Also, since the
price levels change so much, there could possibly be a lot of O(n) price level restructures, since
we have to shift everything around + a lot of different storage changes. A note about this implementation
is that we don't actually remove orders - we only set the quantity to 0, but it gets removed when the
price level gets removed.

The middle is the default implementation, so ordered-map to doubly linked lists. Honestly, not
much to say - I think this is definitely the most reliable and consistent orderbook implementation,
especially when we are considering a matching engine, which this orderbook contains.
I would probably use this orderbook if building a large system just for reliability and it's ability
to stretch amongst different types of securities. 
It provides
very consistent O(logn) price lookup and efficient O(1) removal (true removal, not just setting
quantity to 0). It is also the simplest implementation since we don't have to deal with as much
bookkeeping of our data structures. It falls in the middle of the pack in terms of speed because 
everything is just not fast. Linear is faster because most orders happen towards the inside of the
book so the O(n) worst case turns into ~O(1) average case. This book is guaranteed O(logn), which
is a slight slowdown. RB trees and linked lists have some of the slowest constant
operations as well, which doesn't help (a lot of pointer/memory jumparounds, compare to simple 
addition when considering vectors).

Not much to say about the last implementation maplist. It's unordered map to a CUSTOM linked list
alongside zeroing out the quantity for order cancellation instead of a true removal. I had a lot
of issues with this, I don't even want to consider it one of the implementations, since it's
an inferior version of the default one. It started off as a simple heap allocated array with
one index for every single price level - but I realised this was pretty much impossible since the
prices become very large - most of the runtime would be initializing the actual array - which is
not so bad for a giant orderbook, but our current one is relatively small. Alongside this, I had
no way to iterate through each price level without using a for loop to go down the prices one by
one (even 50 by 50 prices are too slow). So, I decided to use an unordered map to maintain my
goal for this implementation: O(1) price level access. I also needed a set for each price, which
defeated the point of this implementation, because technically every function bcomes O(logn). If not,
how else am I meant to efficiently iterate through levels? A vector would be O(n) since we need it to insert
values sorted. Anyways, it's actually pretty similar to the default implementation. I think the lack
of speed comes mainly from the massive amount of checks I had to do, as well as the O(logn) insertion
in the end. Also, unordered_map runs much slower than just a map for small values, despite being
average O(1) compared to guaranteed O(logn). So this result is not very surprising.


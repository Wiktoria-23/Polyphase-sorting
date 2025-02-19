# Polyphase sorting
<p align="justify">Polyphase sorting is algorithm used to sort large files. It uses tapes that store runs, which are partially sorted data, in files instead of memory. Algorithm consists of distribution data to runs and after that, merging them. Number of runs on each tape used to distribution must be equal to subsequent elements of Fibonacci sequence, because of this merging of runs requires less phases than natural merge.</p>

### Implementation
<p align="justify">This implementation uses two tapes to store distributed runs and one to save result of merge. Data to sort, which are records containing trapeziums data (bases and height), is stored in binary file. During sort, trapeziums are ordered by their surface area. Records can inputed to program manually or they can be generated easily to test file and tapes can be printed after each phase to check how algorithm works. Program counts accesses to disk and prints them at the end of sorting with information about phases needed to complete sort.</p>

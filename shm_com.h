/* A common header file to describe the shared memory we wish to pass about. */

#define TEXT_SZ 2048

struct shared_use_st {
    int written_by_you;
    int nl;
    char some_text[TEXT_SZ];
};


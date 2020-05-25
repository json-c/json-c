
This branch implements the changes briefly described at:

https://github.com/json-c/json-c/wiki/Proposal:-struct-json_object-split

These were originally mentioned in:
Issue #535 - short string optimization: excessive array length

The changes are expected to impact and possibly invalidate:
Issue #552 - Some anlysis about memory allocator in json-c

and will likely cause notable conflicts in any other significant un-merged
changes, such as PR#620 - Introduce json_object_new_string_{ext,noalloc}()



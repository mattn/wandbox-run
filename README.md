# wandbox-run

use [wandbox](http://melpon.org/wandbox/) as shebang

## Usage

```
#!/usr/bin/wandbox-run clang-head

#include <stdio.h>

int
main(int argc, char* argv[]) {
  puts("hello clang");
  return 0;
}
```

```
#!/usr/bin/wandbox-run perl-head

use strict;
use warnings;

warn "hello perl!";
```

If you run wandbox-run in Windows batch file:

```
@wandbox-run clang-head %~f0 %* && exit /b %ERRORLEVEL%
#include <stdio.h>
int main() {
  puts("foo");
}
```

## Requirements

* [json.hpp](https://github.com/nlohmann/json)
* libcurl

## License

MIT

## Author

Yasuhiro Matsumoto (a.k.a. mattn)

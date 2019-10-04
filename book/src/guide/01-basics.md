# Language Basics

Ethereal is, by design, a relatively small language. This is not a shortcoming though.
It is meant to be small since its main purpose is in scripting which usually does not require a huge set of complex features and hence, it's a rather minimal language, focusing primarily on providing decent performance with ease of use.

A simple **hello world** program in Ethereal can be written as:
```perl
println('hello world');
```

Save the code in a file named, say `hello.et`, and run it using the `et` binary which we built in the [installation](./00-install.md) document. Assuming that no `PREFIX` was set, the binary would be created in the `bin/` directory of the cloned repostory. Hence, the command would be:
```bash
./bin/et hello.et
```

This will output `hello world` on the display.

Congratulations! You have successfully written your first program in Ethereal!

# Additional Notes
The `println` function adds a new line at the end of each call, so we did not have to write a new line character (`\n`).
But if we want to explicitly write new line characters wherever required without the function doing that internally, we can use the `print` function for that.

For example, if we want to display multiple statements, using `print`:
```perl
print('first line');
print('second line');
```

The output would be:
```
first linesecond line
```

To correct this, we will add a new line at the end of the first `print` call.
```perl
print('first line\n');
print('second line');
```

which then gives us the correct output:
```
first line
second line
```

## Conclusion
The `print` function is different from `println` in that it will not automatically add new line character at the end. Also, the `print` function requires at least one argument, whereas the `println` function requires no argument - in which case, it will simply enter a new line. For all other intents and purposes, the `print` and `println` functions are identical.
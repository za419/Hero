# VersionControl
A toy version control system I'm working with, purely and entirely for fun

Depends on [PicoSHA2](https://github.com/okdshin/PicoSHA2) for generating hashes -
This is added as a submodule.

Also depends on [date](https://github.com/HowardHinnant/date) for generating 
timestamps - This is also added as a submodule.

## Contributing

Before contributing, please read the [Code of Conduct](CODE_OF_CONDUCT.md) and 
the [Contribution Guidelines](.github/CONTRIBUTING.md).

### Submodules

To contribute to Hero, please note that git submodules are used in the
project. This means that in order to properly clone it, you should pass
`--recursive` to `git clone`, or alternatively you should run these two commands
after cloning:

1. `git submodule init`

2. `git submodule update`

After these are complete, or after a clone with `--recursive`, submodules will be
properly set up.

When working with any project using submodules, you should occasionally run `git
submodule update --remote` to update the submodules. Of course, if you regularly
pull from an upstream source of the project that keeps submodules up to date, this
is a nonissue.

If you set the configuration setting `status.submodulesummary`, ie if you run `git
config status.submodulesummary 1`, then git will generate a short summary of
changes to submodules when running commands like `status`. Additionally, `git
diff` will provide some information about changes in submodules if passed
`--submodule`.

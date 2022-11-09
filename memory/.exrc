lua << EOF
local dap = require('dap')
dap.configurations.c = {
  {
      name = "Launch file",
      type = "cppdbg",
      -- type = "lldb",
      request = "launch",
      -- stopOnEntry = true,
      externalConsole = false,
      program = "./build//${fileBasenameNoExtension}",
      cwd = '${workspaceFolder}',
      setupCommands = {
        {
            text = '-enable-pretty-printing',
            description =  'enable pretty printing',
            ignoreFailures = false 
        },
      },
      },
}
EOF

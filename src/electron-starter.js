// Modules to control application life and create native browser window
const ioHook = require("iohook");
const { app, BrowserWindow } = require("electron");
const fs = require("fs");
const menubar = require("menubar");
const mb = menubar();

// Keep a global reference of the window object, if you don't, the window will
// be closed automatically when the JavaScript object is garbage collected.
let mainWindow;

function initializeDirectories(APP_DIRECTORY, LOG_DIRECTORY) {
  if (!fs.existsSync(APP_DIRECTORY)) {
    fs.mkdirSync(APP_DIRECTORY);
  }
  if (!fs.existsSync(LOG_DIRECTORY)) {
    fs.mkdirSync(LOG_DIRECTORY);
  }
}

function createWindow() {
  // Create the browser window.
  mainWindow = new BrowserWindow({ width: 800, height: 600 });

  // and load the index.html of the app.
  mainWindow.loadURL("http://localhost:3000");

  // Open the DevTools.
  // mainWindow.webContents.openDevTools()

  // Emitted when the window is closed.
  mainWindow.on("closed", function() {
    // Dereference the window object, usually you would store windows
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    mainWindow = null;
  });

  ioHook.start(false);
  const homedir = require("os").homedir(),
    APP_DIRECTORY = homedir + "/.momentum",
    LOG_DIRECTORY = APP_DIRECTORY + "/logs";

  initializeDirectories(APP_DIRECTORY, LOG_DIRECTORY);

  let startTime = new Date().getTime(),
    lastTime = startTime,
    logFilePath = `${LOG_DIRECTORY}/${lastTime}.csv`;

  const logStream = fs.createWriteStream(logFilePath, { flags: "a" });

  logStream.write(`0,${lastTime}\n`);

  ioHook.on("keydown", function(event) {
    const t = new Date().getTime(),
      dT = t - lastTime;
    lastTime = t;

    logStream.write(`${event.keycode},${dT}\n`);
  });
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
mb.on("ready", createWindow);

// Quit when all windows are closed.
app.on("window-all-closed", function() {
  // On OS X it is common for applications and their menu bar
  // to stay active until the user quits explicitly with Cmd + Q
  if (process.platform !== "darwin") {
    app.quit();
  }
});

app.on("activate", function() {
  // On OS X it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (mainWindow === null) {
    createWindow();
  }
});

// In this file you can include the rest of your app's specific main process
// code. You can also put them in separate files and require them here.

# Dependencies
## Boost 1.83.0 (MSVC 14.3) x64 (https://sourceforge.net/projects/boost/files/boost-binaries/1.83.0/boost_1_83_0-msvc-14.3-64.exe/download)

1. Add new environment variable called BOOST_ROOT pointing to installation directory (Avoid the last diagonal symbol \\)

- Example: BOOST_ROOT=C:\local\boost_1_83_0

## MySQL Server Community Edition (https://dev.mysql.com/downloads/mysql/8.0.html)

1. Download the Windows MSI Installer.
2. Scroll down to the bottom and click on "No thanks, just take me to the downloads!"
3. When the installation is almost done, make sure "Launch the MySQL Instance Configuration Wizard" is checked, then click "Finish".
4. When the MySQL Instance Configuration Wizard launches, most default options are fine, but remember the username and password you use (root // whatever). You will need them to log into your chosen database management tool (below) in order to import SQL files later.
5. To test if MySQL is set up correctly, hit CTRL+ALT+DEL on your keyboard, enter the Task Manager, and select the "Services" tab. In the list of services you should see "MySQL" with a status of "Running".
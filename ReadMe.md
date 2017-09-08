> Path to the 'generated' folder inside the storage folder should be defined in DEFINES += RESOURCES_PATH_ inside the XMLGeneratorApp.pro .
>
> Variable named 'PATH_CONFIG_PATH should be added with value /usr/local/Cellar/opencv3/3.2.0/lib/pkgconfig/opencv.pc
>>Use find /usr/local -name "opencv.pc" to get the path.
>
> Add /usr/local/bin to PATH variable to be able to access pkg-config.
>
> Go to Project -> Run -> Run Environment -> Unset DYLD_LIBRARY_PATH
>

# Warning! This code was automatically generated for projects
# without default 'CMakeLists.txt' file.

set(app_sources
	"main.cpp"
	"Accelerometer.cpp"
	"comm.cpp"
	"drone.cpp"
	"Kalman.cpp"
	"magnet.cpp"
	"motor.cpp"
	"Pressure.cpp"
	"utils.cpp"
	"Wifi.cpp"
)

idf_component_register(SRCS ${app_sources})

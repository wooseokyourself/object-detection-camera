{
    "targets": [
        {
            "target_name": "camera",
            "sources": ["src/native/camera.cpp"],
            "link_settings": {
                "libraries": ["-lopencv_core", "-lopencv_highgui", "-lopencv_imgproc", "-lopencv_video", "-lopencv_ml"]
            },
            "cflags": [
                "-g", "-std=c++11", "-Wall"
            ],
            "conditions": [
                ['OS=="linux"', {
                    'include_dirs': [
                        '/usr/include',
                        '/usr/local/include',
                        '/usr/local/include/opencv4'
                        ],
                    'link_settings': {
                        'library_dirs': [
                            '/usr/share/lib', 
                            '/usr/local/include/opencv4'
                            ]
                    },
                    'cflags!': ['-fno-exceptions'],
                    'cflags_cc!': ['-fno-rtti', '-fno-exceptions']
                }],
                ['OS=="mac"', {
                    'include_dirs': [
                        '/opt/local/include',
                        '/usr/local/Cellar/opencv/4.5.0_5/include/opencv4'
                        ],
                    'link_settings': {
                        'library_dirs': [
                            '/opt/local/lib', 
                            '/usr/local/Cellar/opencv/4.5.0_5/lib'
                            ]
                    },
                    'xcode_settings': {
                        'MACOSX_DEPLOYMENT_TARGET' : '10.7',
                        'OTHER_CFLAGS': [
                            "-mmacosx-version-min=10.7",
                            "-std=c++11",
                            "-stdlib=libc++"
                        ],
                        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
                        'GCC_ENABLE_CPP_RTTI': 'YES'
                    }
                }]
            ]
    }
    ]
}
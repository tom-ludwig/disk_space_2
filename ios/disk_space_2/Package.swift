// swift-tools-version: 5.9

import PackageDescription

let package = Package(
    name: "disk_space_2",
    platforms: [
        .iOS("13.0"),
        .macOS("10.15")
    ],
    products: [
        .library(name: "disk-space-2", type: .static, targets: ["disk_space_2"])
    ],
    dependencies: [],
    targets: [
        .target(
            name: "disk_space_2",
            dependencies: [],
            resources: []
        )
    ]
)

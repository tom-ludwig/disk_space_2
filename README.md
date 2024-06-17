# disk_space_2
A Flutter package that provides an easy way to get disk space information on Android and iOS devices.

> [!NOTE]  
> This is a fork of the disk_space package from Marvin Böddeker. Since it was not maintained, we decided to fork it and maintain it ourself. We will be adding new features and fixing bugs as needed.

## Description
This package allows you to get the following information about the device's disk space:

- Free disk space
- Total disk space
- Free disk space for a specific path

## Usage
```dart
import 'package:disk_space/disk_space.dart';

void main() async {
  double? freeDiskSpace = await DiskSpace.getFreeDiskSpace;
  print('Free disk space: $freeDiskSpace');

  double? totalDiskSpace = await DiskSpace.getTotalDiskSpace;
  print('Total disk space: $totalDiskSpace');

  double? freeDiskSpaceForPath = await DiskSpace.getFreeDiskSpaceForPath('/path/to/directory');
  print('Free disk space for path: $freeDiskSpaceForPath');
}
```

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## Contributors
<table>
  <tbody>
    <tr>
      <td><a href="https://github.com/activcoding"><img src="https://avatars.githubusercontent.com/activcoding?v=4?s=100" width="100px" alt="Tom Ludwig" style="border-radius: 15%;"><br /><sub><b>Tom Ludwig</b></sub></a><br/></td>
    </tr>
  </tbody>
</table>

## Original Contributors
<table>
  <tbody>
    <tr>
      <td><a href="https://github.com/mboeddeker"><img src="https://avatars.githubusercontent.com/mboeddeker?v=4?s=100" width="100px" alt="Marvin Böddeker" style="border-radius: 15%;"><br /><sub><b>Marvin Böddeker</b></sub></a><br/></td>
      <td><a href="https://github.com/phipps980316"><img src="https://avatars.githubusercontent.com/phipps980316?v=4?s=100" width="100px" alt="Jon Phipps" style="border-radius: 15%;"><br /><sub><b>Jon Phipps</b></sub></a><br/></td>
    </tr>
  </tbody>
</table>

## License
This project is licensed under the [MIT](LICENSE)
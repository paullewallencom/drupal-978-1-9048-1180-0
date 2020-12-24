version: beta 1

INSTALLATION

The mp3 field is dependent on the getid3 project from Sourceforge:
http://getid3.sourceforge.net/

download the scripts and put them in modules/flexinode/getid3
http://sourceforge.net/project/showfiles.php?group_id=55859&package_id=66206

get the helper applications (for Windows) and put them in modules/flexinode/helperapps
http://sourceforge.net/project/showfiles.php?group_id=55859&package_id=105260

place the field_mp3.inc file in the flexinode directory.

GOALS

The goals of this field are as follows:
1) Allow a restricted set of file types (mp3 and/or other music formats). Currently only mp3 is allowed.
2) Read and display id3 and other metadata
3) automatic creation of an m3u file for streaming from server
4) outputs links to mp3 (for downloading) and m3u (for streaming)

KNOW BUGS AND LIMITATIONS

The Mp3 field's progressive download (streaming) does not work with private downloads (admin/settings)
enabled.

If you edit an existing Mp3 field and replace the file with a different Mp3, the change
does not take place, the update fails. Help fixing this would be greatly appreciated.

TODO

It would be nice if the administrator could change a setting to allow or disallow
further file types. There is also more information available from the id3 tags than is displayed. 
It would be nice id the admin had some control over this as well. I haven't tested
this code with anything but mp3 files, so I don't know what information is 
available to other file formats, but this should be configurable. 

The output of the tag information needs to be considered and possibly improved.

This field is a prime candidate for monitoring by the throttle module, but I
don't have any idea how this is done.

The third party libraries' location should be configurable.

CONTACT

Robert Douglass
rob@robshouse.net

CHANGELOG

2004-10-27 robertDouglass
Added error messages when file is too large or partially uploaded

2004-10-27 Harald Walker
Updated version for Drupal 4.5

2004-10-4 robertDouglass
Fixed spaces in urls problem
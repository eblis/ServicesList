Services list plugin v.0.0.2.8
Copyright � 2006-2009 Cristian Libotean

This plugin will try to list all services that Miranda knows about. It's not possible to list all available services but
this plugin tries to retrieve as many as possible. Please do not change the file name, it will prevent the plugin from detecting
services. Miranda core services cannot be retrieved :(.
It uses a hack and it's not meant to be active all the time, you should only have it enabled for debugging purposes.

Changes:

+ : new feature
* : changed
! : bufgix
- : feature removed or disabled because of pending bugs

version 0.0.2.8 - 2011/08/23
	+ made x64 version updater aware

version 0.0.2.7 - 2009/11/16
	+ x64 support (not tested !!)

version 0.0.2.6 - 2008/01/25
	! Build using VS2005

version 0.0.2.5 - 2008/01/24
	* Changed beta versions server.

version 0.0.2.4 - 2007/05/01
	! Fixed options page entry.

version 0.0.2.3 - 2007/04/24
	* Changed option page to "Services".

version 0.0.2.2 - 2007/03/13
	! Fix possible crash with multiple threads.

version 0.0.2.1 - 2007/03/07
	* Use binary search and quick sort methods - performance should increase a bit.

version 0.0.2.0 - 2007/03/07
	+ Added UUID ( {2f30f130-9749-4523-91c9-28b7e9779dcd} )
	+ Added SERVICESLIST interface.

version 0.0.1.7 - 2007/01/31
	* Changed beta URL.

version 0.0.1.6 - 2007/01/22
	+ Added option to add service as a TopToolBar button (requires TopToolBar plugin and AddTopToolBar buttons (addttbb) plugin).

version 0.0.1.5 - 2007/01/08
	* Requires at least Miranda 0.6 (Removed hack for Miranda 0.5).

version 0.0.1.4 - 2007/01/07
	+ New version resource file.

version 0.0.1.3 - 2006/11/26
	+ Added dll version info.

version 0.0.1.2 - 2006/10/02
	! Fix for multiple plugins hooking CallService()

version 0.0.1.1 - 2006/09/25
	+ Updater support (beta versions)

version 0.0.1.0 - 2006/09/15
	+ Rebased dll (0x2F600000)
	+ Button to refresh stats
	+ Refresh stats on timer
	+ Remember last sort column
	+ Save column widths
	+ Customizable columns
	+ Added patch by Sje.
		Gets the owner of the service.
		Shows the number of times the service was called.
		Shows the total time spent in service call (in seconds).

version 0.0.0.4 - 2006/09/08
	+ Support for Hotkeys+
	+ Ability to copy service name to clipboard.

version 0.0.0.3 - 2006/09/03
	+ Added option for dynamic discovery of services.
	  This will ensure that more services are discovered but will slow down your Miranda a bit.
	+ Refresh list when new services are added.
	* Changed plugin section - now Services

version 0.0.0.2 - 2006/08/27
	* Don't write non transient (most services aren't transient)
	* Rewritten most part of the plugin, now holds internal list of services.
	* Services are displayed in a list in options->plugins
	+ Added service count

version 0.0.0.1 - 2006/08/27
	+ File is overwritten on each run (noone wants 20mb of services)
	+ First release
	
	
	
Translateable string - updated for v. 0.0.0.1.6
;options dialog
[None, do not discover services]
[Static, on service create]
[Dynamic, on service create or call]
[Available services (%d) :]
[Owner]
[Service]
[Address]
[Flags]
[Calls]
[Total time (hi res)]
[Total time]
[Service discovery mode:]
[(restart required)]
[Copy to clipboard]
[Add to Hotkeys+]
[Could not create popup menu]
[Services List autogenerated description]
[Hotkey for service '%s' added successfully]
[There's already a hotkey configured for service '%s']
[Service '%s'does not exist ???]
[Service %s was added to module AddTTBB as entry %s]
;plugin name
[Services list]
[Service plugins]
;misc
[dynamic discovery]
[transient]

// Find the module for the program itself, always at index 0:
const m = Process.enumerateModules()[0];

// The pattern that you are interested in:
const pattern = '55 8b ec 83 ec 10 b8 cc cc cc cc 89 45 f0 89 45 f4 89 45 f8 89 45 fc 8b 45 0c 8b 48';

console.log("[+] Scanning started");
console.log("[+] Base address: ", m.base);
console.log("[+] Module size:  ", m.size);

function doit(pointer) {
	Interceptor.attach(pointer,
	{
		//args: (const StringX &filename, const StringX &passkey, VERSION &version)
		onEnter(args) {
			this.arg0 = Memory.readUtf16String(args[0].readPointer()); // stores the path to the database
			this.arg1 = Memory.readUtf16String(args[1].readPointer()); // stores the password
		},

		//returns an int: 0 correct and 5 is incorrect passkey
		onLeave(retval) {
			if(retval == 0) {
				console.log("[+] Database:  " + this.arg0);
				console.log("[+] Password:  " + this.arg1);
			}
		}
	});
}
  
Memory.scan(m.base, m.size, pattern, {
  onMatch(address, size) {
    console.log('[+] Memory.scan() found match at', address, 'with size', size);
	doit(address);
    // Optionally stop scanning early:
    return 'stop';
  },
  onComplete() {
    console.log('[+] Memory.scan() complete');
  }
});

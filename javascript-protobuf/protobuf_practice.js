var protobuf = require("protobufjs");
protobuf.load("awesome.proto", function(err,root){
	if(err)
				throw err;
	var AwesomeMessage = root.lookupType("awesomepackage.AwesomeMessage");

	var payload = {awesomeField: "AwesomeString"};
	var errMsg = AwesomeMessage.verify(payload);
	if(errMsg)
				throw Error(errMsg);
	// Create a new message
	var message = AwesomeMessage.create(payload);
	//Encode a message to an Unit8Array (browser) or Buffer (node)
	var buffer = AwesomeMessage.encode(message).finish();
    // ... do something with buffer

    // Decode an Uint8Array (browser) or Buffer (node) to a message
    var message = AwesomeMessage.decode(buffer);
    // ... do something with message

    // If the application uses length-delimited buffers, there is also encodeDelimited and decodeDelimited.

    // Maybe convert the message back to a plain object
    var object = AwesomeMessage.toObject(message, {
        longs: String,
        enums: String,
        bytes: String,
        // see ConversionOptions
    });
});


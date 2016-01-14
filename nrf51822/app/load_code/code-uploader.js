#!/usr/bin/env node
var async = require('async');
var noble = require('noble');
var fs = require('fs');

var peripheralId = 'c098e5404536';
if (process.argv.length >= 3) {
    peripheralId = process.argv[2];
}
console.log("Looking for " + peripheralId);

noble.on('stateChange', function(state) {
  if (state === 'poweredOn') {
    console.log("Staring scans...");
    noble.startScanning();
  } else {
    noble.stopScanning();
  }
});

noble.on('discover', function(peripheral) {
  //console.log(peripheral.id);
  if (peripheral.id === peripheralId) {
    noble.stopScanning();

    console.log('peripheral with ID ' + peripheralId + ' found');
    var advertisement = peripheral.advertisement;

    var localName = advertisement.localName;
    var txPowerLevel = advertisement.txPowerLevel;
    var manufacturerData = advertisement.manufacturerData;
    var serviceData = advertisement.serviceData;
    var serviceUuids = advertisement.serviceUuids;

    if (localName) {
      console.log('  Local Name        = ' + localName);
    }

    if (txPowerLevel) {
      console.log('  TX Power Level    = ' + txPowerLevel);
    }

    if (manufacturerData) {
      console.log('  Manufacturer Data = ' + manufacturerData.toString('hex'));
    }

    if (serviceData) {
      console.log('  Service Data      = ' + serviceData);
    }

    if (localName) {
      console.log('  Service UUIDs     = ' + serviceUuids);
    }

    console.log();

    explore(peripheral);
  }
});

function toArrayBuffer(buffer) {
    var ab = newBuffer(buffer.length);
    var view = new Uint8Array(ab);
    for (var i = 0; i < buffer.length; ++i) {
        view[i] = buffer[i];
    }
    return ab;
}

var file_offset = 0;
var file_size = 0;
var file_data;
var characteristic;
var periph;

function writeCallback(err) {
  if (err) throw err;
  file_offset += 20;

  console.log("At offset " + file_offset);
  console.log("File size: " + file_size);
  if (file_offset < file_size-20) {
      characteristic.write(file_data.slice(file_offset, file_offset+20), false, writeCallback);
  } else if (file_offset < file_size) {
      characteristic.write(file_data.slice(file_offset), false, writeCallback);
  } else {
      console.log('Finished');
      var load_code_uuid = '89910c62c3b15d8f7247600c495ed773';

      periph.discoverSomeServicesAndCharacteristics([],[load_code_uuid], function(error, services, characteristics) {
          console.log("Got load code char");
          var load_code_char = characteristics[0];
          var load_data = new Buffer(1);
          load_data[0] = 1;

          load_code_char.write(load_data, false, function(error) {
            console.log("GO!");
            periph.disconnect();
          });
      });
  }
}

function explore(peripheral) {
  peripheral.on('disconnect', function() {
    process.exit(0);
  });

  console.log("Trying to connect");
  peripheral.connect(function(error) {
    console.log('Connected');
    var load_code_uuid = '89910c60c3b15d8f7247600c495ed773';

    peripheral.discoverServices([load_code_uuid], function(error, services) {
        console.log("Discovered service");

        var bin_blob_uuid = '89910c61c3b15d8f7247600c495ed773';
        var load_code_uuid = '89910c62c3b15d8f7247600c495ed773';

        var load_code_service = services[0];
        load_code_service.discoverCharacteristics([bin_blob_uuid], function(error, characteristics) {
            console.log("Characteristics:");
            var bin_blob_char = characteristics[0];

            //var code_buf = new Buffer(4);
            
            fs.readFile('loadable_app/_build/loadable_app_s110.bin', function(err, data) {
                console.log('Opened file');
                file_offset = -20;
                file_size = data.length;
                file_data = data;
                characteristic = bin_blob_char;
                periph = peripheral;

                writeCallback();
            });
        });
    });
  });
}


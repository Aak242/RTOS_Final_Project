import 'dart:io';

import 'package:flutter/material.dart';
import 'package:go_router/go_router.dart';

class WifiPage extends StatefulWidget {
  const WifiPage({super.key});

  @override
  State<WifiPage> createState() => _WifiPageState();
}

class _WifiPageState extends State<WifiPage> {
  List<String> availableNetworks = [];

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            ...availableNetworks.map((network) {
              return TextButton(
                onPressed: () {
                  context.push("/$network");
                },
                child: Text(network),
              );
            }).toList(),
            ElevatedButton(
              onPressed: () async {
                const ip = "192.168.1.10";
                final String subnet = ip.substring(0, ip.lastIndexOf('.'));

                for (var i = 0; i < 256; i++) {
                  String ip = '$subnet.$i';
                  const port = 8080;

                  await Socket.connect(
                    ip,
                    port,
                    timeout: const Duration(milliseconds: 1000),
                  ).then(
                    (socket) {
                      socket.destroy();
                      setState(() {
                        availableNetworks.add("$subnet.$i:$port");
                      });
                    },
                  ).catchError((err) {});
                }
              },
              child: const Text("Search hotspot"),
            )
          ],
        ),
      ),
    );
  }
}

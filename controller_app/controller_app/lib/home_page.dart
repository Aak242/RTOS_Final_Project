import 'package:flutter/material.dart';
import 'package:flutter_joystick/flutter_joystick.dart';
import 'package:web_socket_channel/web_socket_channel.dart';
import 'package:controller_app/util/direction_manager.dart';

class MyHomePage extends StatefulWidget {
  final String title;

  const MyHomePage(this.title, {super.key});

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  @override
  Widget build(BuildContext context) {
    WebSocketChannel channel =
        WebSocketChannel.connect(Uri.parse("ws://${widget.title}"));

    return Scaffold(
      appBar: AppBar(
        backgroundColor: Theme.of(context).colorScheme.inversePrimary,
        title: Text(widget.title),
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: <Widget>[
            StreamBuilder(
              stream: channel.stream,
              builder: (context, snapshot) {
                return Text(snapshot.hasData ? '${snapshot.data}' : '');
              },
            ),
            const SizedBox(height: 24),
            Joystick(listener: (details) {
              final DirectionManager manager = DirectionManager(
                x: -details.x,
                y: -details.y,
              );
              channel.sink.add(manager.computePayload().toInteger().toString());
            })
          ],
        ),
      ),
    );
  }
}

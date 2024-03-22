import 'package:flutter/material.dart';
import 'package:go_router/go_router.dart';
import 'package:controller_app/home_page.dart';
import 'package:controller_app/wifi.dart';

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp.router(
      title: 'Flutter Demo',
      theme: ThemeData(
        colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepPurple),
        useMaterial3: true,
      ),
      routerConfig: GoRouter(
        initialLocation: "/",
        routes: [
          GoRoute(
            path: "/",
            builder: (context, state) {
              return const WifiPage();
            },
          ),
          GoRoute(
            path: "/:id",
            builder: (context, state) {
              return MyHomePage(state.pathParameters["id"] as String);
            },
          )
        ],
      ),
    );
  }
}

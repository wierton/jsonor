## examples

```
Json json("{'12':[1, 2, 45, '45', {'67':'what'}], 'k2':123}");

json["12"].to_string(); // [1, 2, 45, '45', {"67":"what"}]
json["12"][3].to_int(); // 45
json["k2"].to_int();    // 123
```

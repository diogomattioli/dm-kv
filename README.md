# DM-KV

dm-kv is an in-memory key-value database.


It has a concurrent multi-level key-value support, allowing users to query the database simultaneously, growing dynamically without copying or transferring contents from different places in the memory, and with JSON input format.


## Project priorities

1 - Speed

2 - Low memory usage

3 - Dynamic support


## Installation

`git clone` and `make`


## Value format

string - `"my_value"`

number - `123, 456.78, -90`

array  - `["my_value1", "my_value2"]`

object - `{"my_property1" : "my_value1", "my_property2" : "my_value2"}`

other  - `null, true, false`

For more info [json.org](http://json.org/)

## Query format

`[operator]/key/subkey:value`



### Insert

operator `+`, e.g. `+/key:value`

```python
# inserts a key named 'user' with value 'michael'`
+/manager:"michael" 

# inserts a key named 'phone' that supports other keys
+/phone`

# inserts a key named 'mobile' with value '5550000' inside the key named 'phone'
+/phone/mobile:"5550000"

# inserts a key named 'email' with values 0-'d.schrute@dmpaper.com' and 1-'j.halpert@dmpaper.com'
+/email:["d.schrute@dmpaper.com", "j.halpert@dmpaper.com"]

# inserts a key named 'company', subkeys 'manager' as 'michael, 'sales' as 0-'dwight' and 1-'jim', and 'seasons' as number 9
+/company:{"manager" : "michael", "sales" : ["dwight", "jim"], "seasons" : 9}
```



## Update

operator `*`, e.g. `*/key:value`

```python
# updates 'manager' key with value 'andy'
*/manager:"andy"
```

Updates follow the same pattern as inserts, but use operator `*`



## Find

operator `.`, e.g. `./key`

```python
# returns 'OK michael'
./manager

# returns nothing because it's an outer-key
./phone

# returns 'OK 5550000'
./phone/mobile

# returns 'OK d.schrute@dmpaper.com'
./email/0

# returns 'OK j.halpert@dmpaper.com'
./email/1

# returns 'OK michael'
./company/manager

# returns 'OK dwight'
./company/sales/0

# returns 'OK jim'
./company/sales/1

# returns 'OK 9'
./company/seasons
```


## Delete

operator `-`, e.g. `-/key`

```python
# deletes 'manager' key
-/manager

# does nothing because it has a subkey
-/phone

# deletes 'mobile' key
-/phone/mobile

# deletes 'phone' because now it has no subkeys
-/phone
```

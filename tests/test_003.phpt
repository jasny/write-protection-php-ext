--TEST--
Test dynamic properties of a write protected object with magic methods
--FILE--
<?php

class Foo extends WriteProtection
{
    public function __set($name, $value)
    {
        echo "magic set $name to $value\n";
    }

    public function __unset($name)
    {
        echo "magic unset $name\n";
    }

    public function setPublicProperty(string $name, $value): void
    {
        throw new Exception("Shouldn't be called");
    }

    public function unsetPublicProperty(string $name): void
    {
        throw new Exception("Shouldn't be called");
    }
}

$foo = new Foo();

$foo->blue = 100;
$foo->blue = 200;
unset($foo->green);

?>
--EXPECT--
magic set blue to 100
magic set blue to 200
magic unset green

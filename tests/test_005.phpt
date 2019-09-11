--TEST--
Test dynamic properties of a write protected object
--FILE--
<?php

class Foo extends WriteProtection
{
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

echo $foo->blue;
?>
--EXPECT--
200
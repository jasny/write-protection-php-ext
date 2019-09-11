--TEST--
Test protected and private properties of a write protected object
--FILE--
<?php

class Foo extends WriteProtection
{
    private $blue;
    protected $green;

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

try {
    $foo->blue = 100;
} catch (Error $err) {
    echo $err->getMessage(), "\n";
}

try {
    unset($foo->green);
} catch (Error $err) {
    echo $err->getMessage(), "\n";
}

?>
--EXPECT--
Cannot access private property Foo::$blue
Cannot access protected property Foo::$green

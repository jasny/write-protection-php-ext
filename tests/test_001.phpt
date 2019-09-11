--TEST--
Test setting a property of a write protected object
--FILE--
<?php

class Foo extends WriteProtection
{
    public int $bar;

    public function setPublicProperty(string $name, $value): void
    {
        if ($name !== 'bar') {
            throw new Exception("Unexpected property name '$name'");
        }

        if (isset($this->bar) && $this->bar > $value) {
            echo "won't decrease bar to $value\n";
            return;
        }

        echo "set bar to $value\n";
        $this->bar = $value;
    }

    public function unsetPublicProperty(string $name): void
    {
        throw new Exception("Shouldn't be called");
    }
}

$foo = new Foo();

$foo->bar = 0;
$foo->bar = 42;
$foo->bar = 10;
$foo->bar = 42;
$foo->bar = 99;

?>
--EXPECT--
set bar to 0
set bar to 42
won't decrease bar to 10
set bar to 42
set bar to 99

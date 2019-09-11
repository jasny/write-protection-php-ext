--TEST--
Test unsetting a property of a write protected object
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

        $this->bar = $value;
    }

    public function unsetPublicProperty(string $name): void
    {
        if ($name !== 'bar') {
            throw new Exception("Unexpected property name '$name'");
        }

        if ($this->bar % 2 === 0) {
            echo "won't unset {$this->bar} (even number)\n";
            return;
        }

        echo "unset {$this->bar}\n";
        unset($this->bar);
    }
}

$foo = new Foo();

$foo->bar = 42;
unset($foo->bar);

$foo->bar = 99;
unset($foo->bar);

?>
--EXPECT--
won't unset 42 (even number)
unset 99

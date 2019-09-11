<?php

class Foo extends WriteProtection
{
    public int $bar;

    public function setPublicProperty(string $name, $value): void
    {
        $this->{$name} = $value;
    }

    public function unsetPublicProperty(string $name): void
    {
        unset($this->{$name});
    }
}

$foo = new Foo();
$foo->bar = 10;
unset($foo->bar);

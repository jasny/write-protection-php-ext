![improved PHP library](https://user-images.githubusercontent.com/100821/46372249-e5eb7500-c68a-11e8-801a-2ee57da3e5e3.png)

# Write protection for public properties (PHP)

[![Build Status](https://travis-ci.org/improved-php-library/write-protection.svg?branch=master)](https://travis-ci.org/improved-php-library/write-protection)
[![Build status](https://ci.appveyor.com/api/projects/status/7rof1vr8mv4kam17/branch/master?svg=true)](https://ci.appveyor.com/project/jasny/write-protection/branch/master)

When defining properties for a class in PHP, you have the following options;

- **public properties** - These may be set from outside the scope of the class, thus not allowing the class to ensure
  integrity of theobject.
- **getters / setters** will bloat a class which has a fair amount properties. Additionally it reduces readability and
  negatively impacts (read) performance of public properties.
- **magic methods** Using magic `__get`, `__set` and `__unset` methods hide which properties are available. It also
  negatively impacts (read) performance of public properties.

The write protection extension provides a fourth option; If a class extends `WriteProtection`, public properties can be
read as usual. However, when a public property is set or unset from outside of the scope of the class  
`setPublicProperty()` / `unsetPublicPropety()` is called instead.

## :warning: Experimental
This is an experimental PHP extension. Think twice before using it in production.

## Requirements

* PHP 7.4+

## Installation

    git clone https://github.com/improved-php-library/write-protection.git

    phpize
    ./configure
    make
    make test
    make install

Add the following line to your `php.ini`

    extension=skeleton.so


## Examples

### Immutable object

```php
class Foo extends WriteProtection
{
    public int $bar;
    
    public function setPublicProperty(string $name, $value): void
    {
        throw new LogicException("Unable to set Foo::{$name}. Foo objects are immutable");
    }
    
    public function unsetPublicProperty(string $name): void
    {
        throw new LogicException("Unable to unset Foo::{$name}. Foo objects are immutable");
    }
    
    public function __set($name, $value)
    {
        throw new LogicException("Unable to set dynamic property {$name}. Foo objects are immutable");
    }
}
```

### Property guard

```php
class Foo extends WriteProtection
{
    public int $bar;
    
    public function setPublicProperty(string $name, $value): void
    {
        if ($name === 'bar' && is_int($value) && $this->bar > $value) {
            throw new RuntimeException("Won't decrease Foo::bar to $value");
        }
        
        $this->{$name} = $value;
    }
    
    public function unsetPublicProperty(string $name): void
    {
        if ($name === 'bar' && $this->bar > 9000) {
            throw new RuntimeException("Unable to unset Foo::bar; It's already over 9000");
        }
        
        unset($this->{$name});
    }
}
```

### Read-only properties

```php
class Foo extends WriteProtection
{
    /**
     * @readonly
     */ 
    public int $bar;
    
    protected function isReadOnlyProperty(string $name): bool
    {
        $doc = (new ReflectionProperty(get_class($this), $name))->getDocComment();
        
        return strpos("@readonly", $doc) !== false;
    }
    
    public function setPublicProperty(string $name, $value): void
    {
        if ($this->isReadOnlyProperty($name)) {
            throw new LogicException("Unable to set read-only property Foo::{$name}");
        }
        
        $this->{$name} = $value;
    }
    
    public function unsetPublicProperty(string $name): void
    {
        if ($this->isReadOnlyProperty($name)) {
            throw new LogicException("Unable to unset read-only property Foo::{$name}");
        }
        
        unset($this->{$name});
    }
}
```

_Normally you'd parse and cache the property meta data, but that's outside the scope of the example._

## FAQ

#### Why an abstract base class instead of an interface?
Only possible a class defined in a PHP extension can overwrite the handlers for setting and unsetting properties.
Unfortunately, it's not possible to modify or overwrite the standard object handlers to check for an interface.

#### Why aren't properties protected when called by the class or a child class?
Within the class, you're expected to protect the integrity (or break it, if you don't care). Also you need a way to
setting the properties from within the `setPublicProperty()` method would be difficult.

#### Does this affect dynamic properties?
No. Only for properties that are defined and declared as `public` the `setPublicProperty()` method is called. For
dynamic properties the magic `__set()` method is called instead. If that method isn't defined, the properties are
set as usual.

#### Will this feature come to PHP 8?
:man_shrugging: There is no RVC for this feature or a feature like this. If there ever was, it's more likely that it
would be implemented via magic methods (available for any class) than via a base object or interface.

```php
class Foo
{
    public $bar;

    public function __set_public($name, $value)
    {
        // ...    
    }
    
    public function __unset_public($name, $value)
    {
        // ...    
    }
}
```

#### Why?

![question invalid](https://user-images.githubusercontent.com/100821/64738338-3dc36a80-d4ef-11e9-9b10-c8dd7a291213.gif)
